use std::{fmt::Display, str::FromStr};

use anyhow::{anyhow, Result};
use crossterm::event::KeyEvent;
use quietude::{types::{Direction1D, FormattedString, FormattedText}, world::{dialogue::DialogueTree, log::StringStyle, world::World}};
use ratatui::{prelude::Rect, style::{Modifier, Stylize}, text::Line, widgets::{Block, Borders, Paragraph}, Frame};

use super::{control_scheme::{ControlSchemeType, UiKey}, data_builder::{BuilderData, BuilderDest}, traits::Screen, ui_callback::UiCallbackPreset};

pub struct DialogueEditor {
    tree: Option<DialogueTree>,
    pub cursor_pos: usize,
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum ChoiceAttribute {
    Preconditions,
    Outcomes,
    Destination,
}

/* TODO: replace using EnumIter */
static CHOICE_ATTRIBUTES: [ChoiceAttribute; 3] = [
    ChoiceAttribute::Preconditions,
    ChoiceAttribute::Outcomes,
    ChoiceAttribute::Destination,
];

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
        let tree = self.tree.clone().ok_or(anyhow!("tried to get ownership of empty dialogue tree"));
        self.tree = None;

        tree
    }

    pub fn edit_current_entry_attribute(&mut self, data: BuilderData, attr: ChoiceAttribute) -> Result<()> {
        let index = self.cursor_pos;
        if self.cursor_pos == 0 {
            return Err(anyhow!("tried to edit attribute of speaker dialogue"));
        } else {
            let node = self.tree.as_mut().ok_or(anyhow!("tried to edit entry of inactive dialogue tree"))?.get_active_node_mut()?;
            match attr {
                ChoiceAttribute::Preconditions => {
                    if let BuilderData::DialoguePrecondition(precondition) = data {
                        node.push_choice_precondition(index, precondition);
                    }
                }
                ChoiceAttribute::Outcomes => {
                    node.add_choice_outcome(index, &data.try_into()?);
                }
                ChoiceAttribute::Destination => {
                    return Err(anyhow!("tried to set a destination with builder data"));
                }
            }
        }

        Ok(())
    }
    
    pub fn edit_current_entry_destination(&mut self, s: &str) -> Result<()> {
        let mut index = self.cursor_pos;
        if index == 0 {
            return Err(anyhow!("speaker dialogue has no destination"));
        }

        index -= 1;
        self.tree.as_mut().ok_or(anyhow!("tried to edit entry of inactive dialogue tree"))?.get_active_node_mut()?.set_choice_destination(index, s);
        Ok(())
    }
    
    pub fn edit_current_entry_text(&mut self, s: &FormattedString) -> Result<()> {
        if self.cursor_pos == 0 {
            self.tree.as_mut().ok_or(anyhow!("tried to edit entry of inactive dialogue tree"))?.get_active_node_mut()?.set_speaker_dialogue(s);
        } else {
            self.tree.as_mut().ok_or(anyhow!("tried to edit entry of inactive dialogue tree"))?.get_active_node_mut()?.set_choice_text(self.cursor_pos - 1, s.clone());
        }

        Ok(())
    }

    pub fn get_current_entry(&self) -> Result<FormattedString> {
        Ok(if self.cursor_pos == 0 {
            self.tree.as_ref().ok_or(anyhow!("tried to get entry of inactive dialogue tree"))?.get_active_node()?.speaker_dialogue().clone()
        } else {
            self.tree.as_ref().ok_or(anyhow!("tried to get entry of inactive dialogue tree"))?.get_active_node()?.choice_text(self.cursor_pos - 1).clone()
        })
    }

    pub fn current_destination(&self) -> Result<String> {
        Ok(self.tree.as_ref().ok_or(anyhow!("tried to find destination in inactive dialogue tree"))?.get_active_node()?.choice_destination(self.cursor_pos - 1))
    }

    pub fn move_highlight(&mut self, direction: &Direction1D) -> Result<()> {
        let node = self.tree.as_ref().unwrap_or_else(|| panic!("tried to move cursor without actively editing dialogue tree")).get_active_node()?;

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
            },
        }

        Ok(())
    }
    
    pub fn choices_count(&self) -> Result<usize> {
        Ok(self.tree.as_ref().ok_or(anyhow!("tried to access choices of inactive dialogue tree"))?.get_active_node()?.choices_count_unconditional())
    }

    pub fn choice_attributes() -> Vec<ChoiceAttribute> {
        CHOICE_ATTRIBUTES.iter().map(|attr| *attr).collect()
    }
}

impl Screen for DialogueEditor {
    fn update(&mut self, _world: &World) -> Result<()> {
        Ok(())
    }

    fn render(&mut self, frame: &mut Frame, _world: &World, area: Rect) -> Result<()> {
        let node = self.tree.as_ref().unwrap_or_else(|| panic!("tried to render an inactive dialogue tree")).get_active_node()?;

        let speaker_dialogue = node.speaker_dialogue();
        let choices = node.choices_text_unconditional()?;
        
        let s = FormattedString::from(&None, FormattedText::new("Edit speaker dialogue for node", Some(StringStyle::Dim)));
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

        let s = FormattedString::from(&None, FormattedText::new("Add new dialogue option", Some(StringStyle::Dim)));
        lines.push(Line::from(FormattedString::into_spans(&s)).add_modifier(Modifier::ITALIC));

        if self.cursor_pos == choices.len() + 1 {
            let line = lines.pop().unwrap().add_modifier(Modifier::REVERSED).add_modifier(Modifier::ITALIC);
            lines.push(line);
        }

        let block = Block::default().borders(Borders::ALL).title(format!(
            "{}", self.tree.as_ref().unwrap().speaker_name
        ));

        let p = Paragraph::new(lines).block(block);
        frame.render_widget(p, area);
        
        Ok(())
    }

    fn handle_key_events(&mut self, key_event: KeyEvent, scheme: ControlSchemeType, _world: &World)
        -> Option<UiCallbackPreset> {
        let keys = match scheme.keys_from_code(key_event.code) {
            Some(keys) => keys,
            None => return None,
        };

        for key in keys {
            match key {
                UiKey::MoveUp => return Some(UiCallbackPreset::MoveDialogueEditorCursor(Direction1D::Up)),
                UiKey::MoveDown => return Some(UiCallbackPreset::MoveDialogueEditorCursor(Direction1D::Down)),
                UiKey::Confirm => return Some(UiCallbackPreset::DialogueEditorAddEntry),
                UiKey::EditOption => return Some(UiCallbackPreset::DialogueEditorEditEntry),
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

impl TryFrom<BuilderDest> for ChoiceAttribute {
    type Error = anyhow::Error;

    fn try_from(value: BuilderDest) -> std::result::Result<Self, Self::Error> {
        match value {
            BuilderDest::None => Err(anyhow!("cannot convert {value:?} to ChoiceAttribute")),
            BuilderDest::ChoiceAttribute(attr) => Ok(attr),
        }
    }
}

impl FromStr for ChoiceAttribute {
    type Err = anyhow::Error;

    fn from_str(s: &str) -> std::result::Result<Self, Self::Err> {
        match s {
            "Preconditions" => Ok(ChoiceAttribute::Preconditions),
            "Outcomes" => Ok(ChoiceAttribute::Outcomes),
            "Destination" => Ok(ChoiceAttribute::Destination),
            _ => Err(anyhow!("cannot convert {s} to choice attribute")),
        }
    }
}

impl Display for ChoiceAttribute {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ChoiceAttribute::Preconditions => write!(f, "Preconditions"),
            ChoiceAttribute::Outcomes => write!(f, "Outcomes"),
            ChoiceAttribute::Destination => write!(f, "Destination"),
        }
    }
}
