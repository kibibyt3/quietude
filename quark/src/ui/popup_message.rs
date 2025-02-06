use anyhow::Result;
use crossterm::event::KeyEvent;
use quietude::types::{Color, FormattedString};
use ratatui::{layout::Rect, style::Style, text::Line, widgets::{Block, Clear, Paragraph}, Frame};
use serde::{Deserialize, Serialize};
use tui_textarea::TextArea;

use super::{
    control_scheme::{ControlSchemeType, UiKey},
    ui_callback::UiCallbackPreset,
};

pub enum PopupMessage {
    Ok(FormattedString),
    Err(FormattedString),
}

impl PopupMessage {
    pub fn consumes_input(
        &self,
        popup_input: &mut TextArea<'static>,
        key: KeyEvent,
        scheme: &ControlSchemeType,
    ) -> Option<UiCallbackPreset> {
        if scheme.code_yields_key(key.code, UiKey::NoToDialog)
            || scheme.code_yields_key(key.code, UiKey::YesToDialog)
        {
            return Some(UiCallbackPreset::CloseUiPopup);
        }
        None
    }

    pub fn render(&self, f: &mut Frame, area: Rect) -> Result<()> {
        let spans = match self {
            PopupMessage::Ok(s) => FormattedString::into_spans(s),
            PopupMessage::Err(s) => FormattedString::into_spans(s),
        };
        let p = Paragraph::new(Line::from(spans)).block(Block::bordered());
        f.render_widget(Clear, area);
        f.render_widget(p, area);
        Ok(())
    }
}
