use anyhow::Result;
use crossterm::event::KeyEvent;
use quietude::{types::FormattedString, world::world::World};
use ratatui::{
    layout::{Constraint, Direction, Layout},
    prelude::Rect,
    widgets::{Block, Clear},
    Frame,
};
use tui_textarea::TextArea;

use super::{
    control_scheme::{ControlSchemeType, UiKey},
    traits::Screen,
    ui::{Ui, UiState},
    ui_callback::UiCallbackPreset,
};

/* TODO: maybe try to make this struct have a generic for the style of the title, default, and text
 * eventually
 */
#[derive(Default)]
pub struct TextEditor {
    title: String,
    pub text_area: TextArea<'static>,
    pub on_exit: Option<fn(&FormattedString, &mut Ui) -> Result<()>>,
    location: TextEditorLoc,
}

#[derive(Default, Copy, Clone)]
pub enum TextEditorLoc {
    #[default]
    Global,
    DataBuilder,
}

impl TextEditor {
    pub fn new(
        title: &str,
        default: &str,
        location: TextEditorLoc,
        on_exit: fn(&FormattedString, &mut Ui) -> Result<()>,
    ) -> Self {
        TextEditor {
            title: title.to_string(),
            text_area: TextArea::from(vec![default.to_string()]),
            on_exit: Some(on_exit),
            location,
        }
    }

    pub fn text(&self) -> FormattedString {
        let strings: Vec<_> = self
            .text_area
            .lines()
            .iter()
            .map(|line| FormattedString::raw(&None, &line))
            .collect();
        let mut s = FormattedString::raw(&None, "");
        for string in strings {
            s.concatenate(string);
        }

        s
    }
}

impl Screen for TextEditor {
    fn update(&mut self, world: &World) -> Result<()> {
        Ok(())
    }

    fn render(&mut self, frame: &mut Frame, world: &World, area: Rect) -> Result<()> {
        let layout = Layout::default()
            .direction(Direction::Horizontal)
            .constraints([
                Constraint::Length(1),
                Constraint::Min(30),
                Constraint::Length(1),
            ])
            .split(area);
        let text_layout = Layout::default()
            .direction(Direction::Vertical)
            .constraints([
                Constraint::Length(1),
                Constraint::Min(13),
                Constraint::Length(1),
            ])
            .split(layout[1]);

        let b = Block::bordered().title(self.title.to_string());
        frame.render_widget(Clear, area);
        frame.render_widget(b, area);
        frame.render_widget(&self.text_area, text_layout[1]);

        Ok(())
    }

    fn handle_key_events(
        &self,
        key_event: KeyEvent,
        scheme: ControlSchemeType,
        world: &World,
    ) -> Option<UiCallbackPreset> {
        let keys = match scheme.keys_from_code(key_event.code) {
            Some(keys) => keys,
            None => &vec![],
        };

        for key in keys {
            if *key == UiKey::ExitSubmenu {
                return Some(UiCallbackPreset::ConfirmText(self.location));
            }
        }

        return Some(UiCallbackPreset::TextEditorInput(key_event));
    }

    fn refresh_rate(&self) -> u16 {
        60
    }
}
