use std::str::FromStr;

use anyhow::Result;
use log::debug;
use quietude::{
    types::{Coords3D, Direction1D, Direction3D, FormattedString},
    world::{dialogue::DialogueTree, entity::EntityAttribute},
};
use strum::IntoEnumIterator;

use crate::{
    app::App,
    store::{get_save_path, save_project},
    types::Message,
};

use super::{
    choice_menu::{ChoiceMenu, ChoiceMenuLoc},
    chunk_editor::ChunkEditorState,
    data_builder::{BuilderData, BuilderDest},
    dialogue_editor::DialogueAttr,
    text_editor::{TextEditor, TextEditorLoc},
    ui::{PopupState, Ui, UiState},
};

// TODO: make these names more consistent, e.g. ChunkEditorMoveCursor
pub enum UiCallbackPreset {
    MoveChunkEditorCursor(Direction3D),
    MoveEntityViewCursor(Direction1D),
    MoveChoiceMenuCursor(Direction1D, ChoiceMenuLoc),
    ConfirmChoice(ChoiceMenuLoc),
    ExitChoiceMenu(ChoiceMenuLoc),
    EditEntity(Coords3D),
    EditEntityAttribute(EntityAttribute, FormattedString),
    AddEntityAttribute,
    RemoveEntityAttribute(EntityAttribute),
    ConfirmText(TextEditorLoc),
    ExitTextEditor(TextEditorLoc),
    ExitEntityView,
    MoveDialogueEditorCursor(Direction1D),
    DialogueEditorAddEntry,
    DialogueEditorAddAttr,
    DialogueEditorRemoveAttr,
    ExitDialogueEditor,
    CloseUiPopup,
    SaveToDisk,
}

impl UiCallbackPreset {
    pub fn call(&self, app: &mut App) -> Result<Option<Message>> {
        match self {
            UiCallbackPreset::MoveChunkEditorCursor(direction) => app
                .ui
                .chunk_editor
                .cursor
                .coords
                .move_in_direction(direction),
            UiCallbackPreset::MoveEntityViewCursor(direction) => {
                app.ui.chunk_editor.entity_view.move_cursor(direction)
            }
            UiCallbackPreset::MoveChoiceMenuCursor(direction, loc) => match loc {
                ChoiceMenuLoc::Global => app.ui.choice_menu.move_cursor(*direction),
                ChoiceMenuLoc::DataBuilder => {
                    app.ui.data_builder.choice_menu.move_cursor(*direction)
                }
            },
            UiCallbackPreset::EditEntity(coords) => app
                .ui
                .chunk_editor
                .edit_entity(*coords, &app.world.active_chunk)?,
            UiCallbackPreset::EditEntityAttribute(attr, default) => match attr {
                EntityAttribute::Text(attr) => {
                    let cb = |s: &FormattedString, ui: &mut Ui| -> Result<()> {
                        let (attr, _) = ui.chunk_editor.entity_view.get_current_av_pair()?;
                        if let EntityAttribute::Text(attr) = attr {
                            ui.chunk_editor
                                .entity_view
                                .set_text_attr(attr, &s.to_string());
                        }
                        Ok(())
                    };
                    app.ui.text_editor = TextEditor::new(
                        &format!("{attr}"),
                        &default.to_string(),
                        TextEditorLoc::Global,
                        cb,
                    );
                    app.ui.popup_state = Some(PopupState::TextEditor);
                }
                EntityAttribute::Choice(attr) => {
                    let choices = attr.choices();
                    let cb = |s: &str, ui: &mut Ui| {
                        let (attr, _) = ui.chunk_editor.entity_view.get_current_av_pair()?;
                        if let EntityAttribute::Choice(attr) = attr {
                            ui.chunk_editor
                                .entity_view
                                .set_choice_attr(attr.clone(), s)?;
                        }
                        Ok(None)
                    };
                    app.ui.choice_menu = ChoiceMenu::new(
                        choices.iter().map(|s| String::from(*s)).collect(),
                        ChoiceMenuLoc::Global,
                        cb,
                    );
                    app.ui.popup_state = Some(PopupState::ChoiceMenu);
                }
                EntityAttribute::Dialogue => {
                    let name = app.ui.chunk_editor.entity_view.entity_name()?;
                    app.ui
                        .dialogue_editor
                        .start(&DialogueTree::load_or_init(&name, &get_save_path())?);
                    app.ui.state = UiState::Dialogue;
                }
            },
            UiCallbackPreset::AddEntityAttribute => {
                let ev = &app.ui.chunk_editor.entity_view;
                let choices: Vec<&EntityAttribute> = EntityAttribute::attribute_order()
                    .iter()
                    .filter(|attr| !ev.has_attribute(attr).unwrap())
                    .collect();
                let choices = choices.iter().map(|attr| format!("{attr}")).collect();
                let cb = |s: &str, ui: &mut Ui| {
                    ui.chunk_editor.entity_view.add_attribute(&s.parse()?)?;
                    Ok(None)
                };
                app.ui.choice_menu = ChoiceMenu::new(choices, ChoiceMenuLoc::Global, cb);
                app.ui.popup_state = Some(PopupState::ChoiceMenu);
            }
            UiCallbackPreset::RemoveEntityAttribute(attr) => {
                app.ui.chunk_editor.entity_view.remove_attribute(attr)?;
                app.ui.chunk_editor.entity_view.validate_cursor_pos();
            }
            UiCallbackPreset::ConfirmText(loc) => match loc {
                TextEditorLoc::Global => {
                    let s = app.ui.text_editor.text();
                    app.ui.text_editor.on_exit.take().unwrap()(&s, &mut app.ui)?;
                    app.ui.popup_state = None;
                }
                TextEditorLoc::DataBuilder => {
                    let s = app.ui.data_builder.text_editor.text();
                    app.ui.data_builder.text_editor.on_exit.take().unwrap()(&s, &mut app.ui)?;
                    app.ui.popup_state = None;
                    app.ui.data_builder.cb.take().unwrap()(app.ui.data_builder.data()?, &mut app.ui)?;
                    app.ui.data_builder.reset();
                }
            },
            UiCallbackPreset::ConfirmChoice(loc) => match loc {
                ChoiceMenuLoc::Global => {
                    let s = &app.ui.choice_menu.options[app.ui.choice_menu.index].clone();
                    let dest = app.ui.choice_menu.on_exit.take().unwrap()(s, &mut app.ui)?;
                    app.ui.popup_state = dest;
                }
                ChoiceMenuLoc::DataBuilder => {
                    let s = &app.ui.data_builder.choice_menu.options
                        [app.ui.data_builder.choice_menu.index]
                        .clone();
                    app.ui.popup_state =
                        app.ui.data_builder.choice_menu.on_exit.take().unwrap()(s, &mut app.ui)?;
                    if app.ui.popup_state.is_none() {
                        app.ui.data_builder.cb.take().unwrap()(app.ui.data_builder.data()?, &mut app.ui)?;
                        app.ui.data_builder.reset();
                    }
                }
            },
            UiCallbackPreset::ExitTextEditor(loc) => match loc {
                TextEditorLoc::Global => app.ui.popup_state = None,
                TextEditorLoc::DataBuilder => app.ui.data_builder.state = None,
            },
            UiCallbackPreset::ExitChoiceMenu(loc) => match loc {
                ChoiceMenuLoc::Global => app.ui.popup_state = None,
                ChoiceMenuLoc::DataBuilder => app.ui.data_builder.state = None,
            },
            UiCallbackPreset::ExitEntityView => {
                let entity = app.ui.chunk_editor.entity_view.finish()?;
                let coords = entity.coords.clone();
                app.next_valid_entity_id = app.world.active_chunk.overwrite_entity(
                    entity,
                    &coords,
                    app.next_valid_entity_id,
                );
                app.ui.chunk_editor.state = ChunkEditorState::Main;
            }
            UiCallbackPreset::MoveDialogueEditorCursor(direction) => {
                app.ui.dialogue_editor.move_highlight(direction)?
            }
            UiCallbackPreset::DialogueEditorAddEntry => {
                let cb = |s: &FormattedString, ui: &mut Ui| -> Result<()> {
                    ui.dialogue_editor.edit_current_entry_text(s)?;
                    Ok(())
                };

                let index = app.ui.dialogue_editor.cursor_pos;
                let title = if index == 0 {
                    "Speaker Dialogue".to_string()
                } else {
                    format!("Option {index} Text")
                };

                let default = app.ui.dialogue_editor.get_current_entry()?;

                app.ui.text_editor =
                    TextEditor::new(&title, &default.to_string(), TextEditorLoc::Global, cb);
                app.ui.popup_state = Some(PopupState::TextEditor);
            }
            UiCallbackPreset::DialogueEditorAddAttr => {
                let cb = |s: &str, ui: &mut Ui| {
                    let attr = DialogueAttr::from_str(s)?;
                    let index = ui.dialogue_editor.cursor_pos;
                    if let DialogueAttr::Destination(_) = attr {
                        let cb = |s: &FormattedString, ui: &mut Ui| -> Result<()> {
                            ui.dialogue_editor.add_entry_attr(
                                DialogueAttr::Destination(s.to_string()),
                            )?;
                            Ok(())
                        };
                        let title = format!("Option {index} Destination");
                        ui.text_editor = TextEditor::new(
                            &title,
                            &ui.dialogue_editor.current_destination()?,
                            TextEditorLoc::Global,
                            cb,
                        );
                        Ok(Some(PopupState::TextEditor))
                    } else {
                        let cb = |dest: BuilderDest, ui: &mut Ui| -> Result<()> {
                            let attr = dest.try_into()?;
                            ui.dialogue_editor.add_entry_attr(attr)?;
                            Ok(())
                        };
                        let dest = attr.clone().into();
                        let data = match attr {
                            DialogueAttr::Precondition(_) => {
                                BuilderData::DialoguePrecondition(Default::default())
                            }
                            DialogueAttr::Outcome(_) => {
                                BuilderData::DialogueOutcome(Default::default())
                            }
                            DialogueAttr::Destination(_) => unreachable!(),
                        };
                        ui.data_builder.build(data, dest, cb)?;
                        Ok(Some(PopupState::DataBuilder))
                    }
                };

                let index = app.ui.dialogue_editor.cursor_pos;
                if index != 0 && index != app.ui.dialogue_editor.choices_count()? + 1 {
                    let title = format!("Option {index} Attributes");
                    let choices = DialogueAttr::iter()
                        .map(|choice| choice.to_string())
                        .collect();
                    app.ui.choice_menu = ChoiceMenu::new(choices, ChoiceMenuLoc::Global, cb);
                    app.ui.choice_menu.abbreviate_choices('(');
                    app.ui.popup_state = Some(PopupState::ChoiceMenu);
                }
            }
            UiCallbackPreset::DialogueEditorRemoveAttr => {
                let cb = |s: &str, ui: &mut Ui| {
                    let attr = s.parse()?;
                    ui.dialogue_editor.remove_entry_attr(attr)?;
                    Ok(None)
                };
                let choices = app
                    .ui
                    .dialogue_editor
                    .removeable_attrs()?
                    .iter()
                    .map(|attr| attr.to_string())
                    .collect();
                
                app.ui.choice_menu = ChoiceMenu::new(choices, ChoiceMenuLoc::Global, cb);
                app.ui.popup_state = Some(PopupState::ChoiceMenu);
            }
            UiCallbackPreset::ExitDialogueEditor => {
                let tree = app.ui.dialogue_editor.finish()?;
                let path = app.project_dir.to_path_buf();

                tree.save(&path)?;
                app.ui.state = UiState::Chunk;
            }
            UiCallbackPreset::CloseUiPopup => app.ui.close_popup(),
            UiCallbackPreset::SaveToDisk => save_project(app)?,
        }

        Ok(None)
    }
}
