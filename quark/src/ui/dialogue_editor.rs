use anyhow::{anyhow, Result};
use crossterm::event::KeyEvent;
use parse_display::{Display, FromStr};
use quietude::{
    types::{Direction1D, FormattedString, FormattedText},
    world::{
        dialogue::{DialogueOutcome, DialoguePrecondition, DialogueTree},
        log::StringStyle,
        world::World,
    },
};
use ratatui::{
    prelude::Rect,
    style::{Modifier, Stylize},
    text::Line,
    widgets::{Block, Borders, Paragraph},
    Frame,
};
use strum::EnumIter;

use super::{
    control_scheme::{ControlSchemeType, UiKey},
    data_builder::{BuilderData, BuilderDest},
    traits::Screen,
    ui_callback::UiCallbackPreset,
};

pub struct DialogueEditor {
    tree: Option<DialogueTree>,
    pub cursor_pos: usize,
}

#[derive(FromStr, Display, EnumIter, Debug, PartialEq, Clone, Hash, Eq)]
#[display("{}({0})")]
pub enum DialogueAttr {
    Precondition(DialoguePrecondition),
    Outcome(DialogueOutcome),
    Destination(String),
}

impl DialogueEditor {
    pub fn new() -> Self {
        DialogueEditor {
            tree: None,
            cursor_pos: 0,
        }
    }

    pub fn start(&mut self, tree: &DialogueTree) {
        self.cursor_pos = 0;
        self.tree = Some(tree.clone());
    }

    pub fn finish(&mut self) -> Result<DialogueTree> {
        let tree = self
            .tree
            .clone()
            .ok_or(anyhow!("tried to get ownership of empty dialogue tree"));
        self.tree = None;

        tree
    }

    pub fn add_entry_attr(&mut self, attr: DialogueAttr) -> Result<()> {
        let mut index = self.cursor_pos;
        if index == 0 {
            return Err(anyhow!("cannot add attribute {attr} to speaker dialogue"));
        } else {
            index -= 1;
            let node = self
                .tree
                .as_mut()
                .ok_or(anyhow!(
                    "cannot add attribute {attr} to inactive dialogue tree"
                ))?
                .get_active_node_mut()?;
            match attr {
                DialogueAttr::Precondition(precondition) => {
                    node.push_choice_precondition(index, precondition);
                }
                DialogueAttr::Outcome(outcome) => {
                    node.push_choice_outcome(index, outcome);
                }
                DialogueAttr::Destination(s) => {
                    node.set_choice_destination(index, &s);
                }
            }
        }
        Ok(())
    }

    pub fn remove_entry_attr(&mut self, attr: DialogueAttr) -> Result<()> {
        let mut index = self.cursor_pos;
        if index == 0 {
            Err(anyhow!(
                "cannot remove attribute {attr} from speaker dialogue"
            ))
        } else {
            index -= 1;
            let node = self
                .tree
                .as_mut()
                .ok_or(anyhow!(
                    "cannot remove attribute {attr} from inactive dialogue tree"
                ))?
                .get_active_node_mut()?;
            match &attr {
                DialogueAttr::Precondition(precondition) => {
                    node.remove_choice_precondition(index, precondition);
                    Ok(())
                }
                DialogueAttr::Outcome(outcome) => {
                    node.remove_choice_outcome(index, outcome);
                    Ok(())
                }
                DialogueAttr::Destination(s) => {
                    Err(anyhow!("cannot remove dialogue destination {s}"))
                }
            }
        }
    }

    pub fn removeable_attrs(&self) -> Result<Vec<DialogueAttr>> {
        let mut index = self.cursor_pos;
        if index == 0 {
            Err(anyhow!("speaker dialogue does not have attributes"))
        } else {
            index -= 1;
            let node = self
                .tree
                .as_ref()
                .ok_or(anyhow!(
                    "cannot get removeable attributes from inactive dialogue tree"
                ))?
                .get_active_node()?;
            let precondition_choices = node.choice_preconditions(index);
            let outcome_choices = node.choice_outcomes(index);

            let mut precondition_choices: Vec<_> = precondition_choices
                .iter()
                .map(|precondition| DialogueAttr::Precondition(precondition.clone()))
                .collect();
            let mut outcome_choices: Vec<_> = outcome_choices
                .iter()
                .map(|outcome| DialogueAttr::Outcome(outcome.clone()))
                .collect();

            precondition_choices.append(&mut outcome_choices);
            Ok(precondition_choices)
        }
    }

    pub fn edit_current_entry_text(&mut self, s: &FormattedString) -> Result<()> {
        if self.cursor_pos == 0 {
            self.tree
                .as_mut()
                .ok_or(anyhow!("tried to edit entry of inactive dialogue tree"))?
                .get_active_node_mut()?
                .set_speaker_dialogue(s);
        } else {
            self.tree
                .as_mut()
                .ok_or(anyhow!("tried to edit entry of inactive dialogue tree"))?
                .get_active_node_mut()?
                .set_choice_text(self.cursor_pos - 1, s.clone());
        }

        Ok(())
    }

    pub fn get_current_entry(&self) -> Result<FormattedString> {
        Ok(if self.cursor_pos == 0 {
            self.tree
                .as_ref()
                .ok_or(anyhow!("tried to get entry of inactive dialogue tree"))?
                .get_active_node()?
                .speaker_dialogue()
                .clone()
        } else {
            self.tree
                .as_ref()
                .ok_or(anyhow!("tried to get entry of inactive dialogue tree"))?
                .get_active_node()?
                .choice_text(self.cursor_pos - 1)
                .clone()
        })
    }

    pub fn current_destination(&self) -> Result<String> {
        Ok(self
            .tree
            .as_ref()
            .ok_or(anyhow!(
                "tried to find destination in inactive dialogue tree"
            ))?
            .get_active_node()?
            .choice_destination(self.cursor_pos - 1))
    }

    pub fn move_highlight(&mut self, direction: &Direction1D) -> Result<()> {
        let node = self
            .tree
            .as_ref()
            .unwrap_or_else(|| {
                panic!("tried to move cursor without actively editing dialogue tree")
            })
            .get_active_node()?;

        match direction {
            Direction1D::Up => {
                if self.cursor_pos > 0 {
                    self.cursor_pos -= 1;
                }
            }
            Direction1D::Down => {
                if self.cursor_pos < node.choices_count_unconditional() + 1 {
                    self.cursor_pos += 1;
                }
            }
        }

        Ok(())
    }

    pub fn choices_count(&self) -> Result<usize> {
        Ok(self
            .tree
            .as_ref()
            .ok_or(anyhow!("tried to access choices of inactive dialogue tree"))?
            .get_active_node()?
            .choices_count_unconditional())
    }
}

impl Screen for DialogueEditor {
    fn update(&mut self, _world: &World) -> Result<()> {
        Ok(())
    }

    fn render(&mut self, frame: &mut Frame, _world: &World, area: Rect) -> Result<()> {
        let node = self
            .tree
            .as_ref()
            .unwrap_or_else(|| panic!("tried to render an inactive dialogue tree"))
            .get_active_node()?;

        let speaker_dialogue = node.speaker_dialogue();
        let choices = node.choices_text_unconditional()?;

        let s = FormattedString::from(
            &None,
            FormattedText::new("Edit speaker dialogue for node", Some(StringStyle::Dim)),
        );
        let mut speaker_line = if speaker_dialogue.to_string() == "" {
            Line::from(FormattedString::into_spans(&s)).add_modifier(Modifier::ITALIC)
        } else {
            Line::from(FormattedString::into_spans(speaker_dialogue))
        };

        if self.cursor_pos == 0 {
            speaker_line = speaker_line.add_modifier(Modifier::REVERSED);
        }

        let mut lines = vec![
            speaker_line,
            Line::from(""),
            Line::from("-------------"),
            Line::from(""),
        ];

        for (i, choice) in choices.iter().enumerate() {
            let spans = FormattedString::into_spans(choice);
            let mut line = Line::from(spans);
            if self.cursor_pos > 0 && self.cursor_pos - 1 == i {
                line = line.add_modifier(Modifier::REVERSED);
            }
            lines.push(line);
        }

        let s = FormattedString::from(
            &None,
            FormattedText::new("Add new dialogue option", Some(StringStyle::Dim)),
        );
        lines.push(Line::from(FormattedString::into_spans(&s)).add_modifier(Modifier::ITALIC));

        if self.cursor_pos == choices.len() + 1 {
            let line = lines
                .pop()
                .unwrap()
                .add_modifier(Modifier::REVERSED)
                .add_modifier(Modifier::ITALIC);
            lines.push(line);
        }

        let block = Block::default()
            .borders(Borders::ALL)
            .title(format!("{}", self.tree.as_ref().unwrap().speaker_name));

        let p = Paragraph::new(lines).block(block);
        frame.render_widget(p, area);

        Ok(())
    }

    fn handle_key_events(
        &mut self,
        key_event: KeyEvent,
        scheme: ControlSchemeType,
        _world: &World,
    ) -> Option<UiCallbackPreset> {
        let keys = match scheme.keys_from_code(key_event.code) {
            Some(keys) => keys,
            None => return None,
        };

        for key in keys {
            match key {
                UiKey::MoveUp => {
                    return Some(UiCallbackPreset::MoveDialogueEditorCursor(Direction1D::Up))
                }
                UiKey::MoveDown => {
                    return Some(UiCallbackPreset::MoveDialogueEditorCursor(
                        Direction1D::Down,
                    ))
                }
                UiKey::Confirm => return Some(UiCallbackPreset::DialogueEditorAddEntry),
                UiKey::EditOption => return Some(UiCallbackPreset::DialogueEditorAddAttr),
                UiKey::RemoveItem => return Some(UiCallbackPreset::DialogueEditorRemoveAttr),
                UiKey::ExitSubmenu => return Some(UiCallbackPreset::ExitDialogueEditor),
                _ => {}
            }
        }

        None
    }

    fn refresh_rate(&self) -> u16 {
        60
    }
}

impl TryFrom<BuilderDest> for DialogueAttr {
    type Error = anyhow::Error;

    fn try_from(value: BuilderDest) -> std::result::Result<Self, Self::Error> {
        match value {
            BuilderDest::None => Err(anyhow!("cannot convert {value:?} to ChoiceAttribute")),
            BuilderDest::DialogueAttr(attr) => Ok(attr),
        }
    }
}

#[cfg(test)]
mod tests {
    use quietude::world::{
        conditions::WorldCondition,
        item::{BookType, ItemType},
    };

    use crate::utils::UnsortedEq;

    use super::*;

    #[test]
    fn add_and_remove_attrs() {
        let mut editor = DialogueEditor::new();
        let tree = DialogueTree::new("test");
        editor.start(&tree);
        editor.cursor_pos += 1;
        editor.edit_current_entry_text(&FormattedString::default());

        let precondition = DialogueAttr::Precondition(DialoguePrecondition::InterlocutorHasItem(
            ItemType::Book(BookType::Babel),
        ));
        let outcome = DialogueAttr::Outcome(DialogueOutcome::RemoveWorldCondition(
            WorldCondition::DiscoveredTimeIsles,
        ));
        editor.add_entry_attr(precondition.clone()).unwrap();
        editor.add_entry_attr(outcome.clone()).unwrap();

        let attrs = vec![precondition.clone(), outcome.clone()];
        assert!(attrs.unsorted_eq(editor.removeable_attrs().unwrap()));

        editor.remove_entry_attr(precondition.clone());
        let attrs = vec![outcome.clone()];
        assert!(attrs.unsorted_eq(editor.removeable_attrs().unwrap()));
    }
}
