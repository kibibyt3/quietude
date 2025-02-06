use std::default;

use ratatui::style::Style;
use serde::{Deserialize, Serialize};

use crate::types::{Color, Coords3D, FormattedString, FormattedText};

#[derive(Default, Debug, Serialize, Deserialize, PartialEq)]
pub struct Log {
    pub contents: Vec<FormattedString>,
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub enum StringStyle {
    Emphasis(Color),
    Dim,
    Attribute,
    Value,
    Error,
}

impl Log {
    pub fn new() -> Self {
        Log {
            contents: vec![],
        }
    }

    pub fn print_formatted_string(&mut self, string: FormattedString) {
        self.contents.push(string);
    }

}

impl StringStyle {
    pub fn to_style(&self) -> Style {
        match self {
            StringStyle::Emphasis(color) => todo!(),
            StringStyle::Dim => Style::new().fg(ratatui::style::Color::DarkGray).bg(ratatui::style::Color::Black),
            StringStyle::Attribute => Style::default().fg(ratatui::style::Color::Yellow).bg(ratatui::style::Color::DarkGray),
            StringStyle::Value => Style::default().fg(ratatui::style::Color::Cyan),
            StringStyle::Error => Style::default().fg(ratatui::style::Color::Red),
        }
    }
}
