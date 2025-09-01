use std::{collections::HashMap, fmt::{Debug, Display, Formatter}};

use anyhow::Result;
use crossterm::event::{KeyCode, KeyEvent, KeyModifiers};
use quietude::world::world::World;
use ratatui::{layout::Rect, Frame};

use super::{control_scheme::{ControlSchemeType, UiKey}, ui_callback::UiCallbackPreset};

pub trait Screen {
    fn update(&mut self, world: &World) -> Result<()>;

    fn render(&mut self, frame: &mut Frame, world: &World, area: Rect) -> Result<()>;

    fn handle_key_events(
        &self,
        key_event: KeyEvent,
        scheme: ControlSchemeType,
        world: &World,
    ) -> Option<UiCallbackPreset>;

    fn refresh_rate(&self) -> u16;

    fn controls_debug(&self, scheme: ControlSchemeType, world: &World)
            -> HashMap<KeyCode, UiCallbackPreset> {
        let mut controls = HashMap::new();
        for keycode in scheme.mapped_keys() {
            if let Some(cb) = self.handle_key_events(
                KeyEvent::new(*keycode, KeyModifiers::empty()),
                scheme,
                world,
            ) {
                controls.insert(*keycode, cb);
            }
        }
        controls
    }
}

impl Debug for dyn Screen {
    fn fmt(&self, f: &mut Formatter<'_>) -> core::fmt::Result {
        write!(f, "Screen {:?}", self)
    }
}
