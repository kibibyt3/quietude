use anyhow::Result;
use crossterm::event::KeyEvent;
use quietude::{
    types::Direction1D,
    world::{log::StringStyle, world::World},
};
use ratatui::{
    prelude::Rect,
    style::{Modifier, Stylize},
    text::Line,
    widgets::{Block, Clear, Paragraph},
    Frame,
};

use super::{
    control_scheme::{ControlSchemeType, UiKey},
    traits::Screen,
    ui::{PopupState, Ui},
    ui_callback::UiCallbackPreset,
};

#[derive(Default)]
pub struct ChoiceMenu {
    pub index: usize,
    pub options: Vec<String>,
    pub on_exit: Option<fn(&str, &mut Ui) -> Result<Option<PopupState>>>,
    pub location: ChoiceMenuLoc,
    abbr_delim: Option<char>,
}

#[derive(Default, Clone, Copy)]
pub enum ChoiceMenuLoc {
    #[default]
    Global,
    DataBuilder,
}

impl ChoiceMenu {
    pub fn new(
        options: Vec<String>,
        location: ChoiceMenuLoc,
        on_exit: fn(&str, &mut Ui) -> Result<Option<PopupState>>,
    ) -> Self {
        ChoiceMenu {
            index: 0,
            options,
            on_exit: Some(on_exit),
            location,
            abbr_delim: None,
        }
    }

    pub fn abbreviate_choices(&mut self, delim: char) {
        self.abbr_delim = Some(delim);
    }

    pub fn move_cursor(&mut self, direction: Direction1D) {
        match direction {
            Direction1D::Up => {
                if self.index > 0 {
                    self.index -= 1;
                }
            }
            Direction1D::Down => {
                if self.index < self.options.len() - 1 {
                    self.index += 1;
                }
            }
        }
    }

    pub fn get_cursor_pos(&self) -> usize {
        self.index
    }
}

impl Screen for ChoiceMenu {
    fn update(&mut self, _world: &World) -> Result<()> {
        Ok(())
    }

    fn render(&mut self, frame: &mut Frame, _world: &World, area: Rect) -> Result<()> {
        let mut lines = vec![];

        let mut choices = self.options.clone();
        if let Some(delimiter) = self.abbr_delim {
            choices = choices
                .iter()
                .map(|s| match s.split_once(delimiter) {
                    Some((s, _)) => s.to_string(),
                    None => s.to_string(),
                })
                .collect();
        }

        for (i, option) in choices.iter().enumerate() {
            let mut line = Line::styled(option, StringStyle::Value.to_style());
            if i == self.index {
                line = line.add_modifier(Modifier::REVERSED);
            }
            lines.push(line);
        }

        let p = Paragraph::new(lines).block(Block::bordered());
        frame.render_widget(Clear, area);
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
            None => &vec![],
        };

        for key in keys {
            match key {
                UiKey::MoveDown => {
                    return Some(UiCallbackPreset::MoveChoiceMenuCursor(
                        Direction1D::Down,
                        self.location,
                    ));
                }
                UiKey::MoveUp => {
                    return Some(UiCallbackPreset::MoveChoiceMenuCursor(
                        Direction1D::Up,
                        self.location,
                    ));
                }
                UiKey::Confirm => {
                    return Some(UiCallbackPreset::ConfirmChoice(self.location));
                }
                UiKey::ExitSubmenu => {
                    return Some(UiCallbackPreset::ExitChoiceMenu(self.location));
                }
                _ => {}
            }
        }

        return None;
    }

    fn refresh_rate(&self) -> u16 {
        60
    }
}
