use ratatui::layout::{Constraint, Direction, Layout, Rect};
use tui_textarea::TextArea;

use super::{
    constants::{MAX_NAME_LENGTH, MIN_NAME_LENGTH},
    widgets::default_block,
};

pub fn validate_textarea_input(textarea: &mut TextArea<'_>, title: String) -> bool {
    let text = textarea.lines()[0].trim();
    if text.len() < MIN_NAME_LENGTH {
        textarea.set_block(default_block().title(title).title("(too short)"));
        false
    } else if text.len() > MAX_NAME_LENGTH {
        textarea.set_block(default_block().title(title).title("(too long)"));
        false
    } else {
        textarea.set_block(default_block().title(title));
        true
    }
}

pub fn centered_rect(r: Rect, padding_x: u16, padding_y: u16) -> Rect {
    let layout = Layout::new(Direction::Horizontal, vec![
        Constraint::Length(padding_x),
        Constraint::Min(1),
        Constraint::Length(padding_x),
    ]).split(r);

    Layout::new(Direction::Vertical, vec![
        Constraint::Length(padding_y),
        Constraint::Min(1),
        Constraint::Length(padding_y),
    ]).split(layout[1])[1]
}
