use std::fmt::Display;

use anyhow::{anyhow, Result};
use crossterm::event::KeyEvent;
use log::debug;
use parse_display::{Display, FromStr};
use quietude::{
    types::FormattedString, ui::utils::centered_rect, world::{
        conditions::WorldCondition,
        dialogue::{DialogueOutcome, DialoguePrecondition},
        item::{ArmourType, BookType, ItemType, WeaponType},
        world::World,
    }
};
use ratatui::{prelude::Rect, widgets::Block, Frame};
use serde::Serialize;
use strum::IntoEnumIterator;

use super::{
    choice_menu::{ChoiceMenu, ChoiceMenuLoc},
    control_scheme::ControlSchemeType,
    dialogue_editor::ChoiceAttribute,
    text_editor::{TextEditor, TextEditorLoc},
    traits::Screen,
    ui::{PopupState, Ui},
    ui_callback::UiCallbackPreset,
};

#[derive(Default)]
// TODO: in the long run it might be better to have everything happen in one menu
// this would involve getting rid of ChoiceMenuLoc and TextEditorLoc
// e.g.
// ______________________________
// |                             |
// |   Opt1                      |
// | > Opt2(_)                   |
// |   |--InnerOpt1              |
// |   |--InnerOpt2              |
// |   |--InnerOpt3(_)           |
// |   |  |--InnerInner1         |
// |   |  |--InnerInner2         |
// |   Opt3                      |
// |                             |
// ______________________________

pub struct DataBuilder {
    data: Option<BuilderData>,
    pub state: Option<BuilderState>,
    cb: Option<fn(BuilderData, BuilderDest, &mut Ui) -> Result<()>>,
    depth: usize,
    pub choice_menu: ChoiceMenu,
    pub text_editor: TextEditor,
    pub dest: Option<BuilderDest>,
}

#[derive(Default, Copy, Clone)]
pub enum BuilderState {
    #[default]
    Choice,
    Text,
}

#[derive(Serialize, FromStr, Display)]
#[display("{0}")]
pub enum BuilderData {
    DialoguePrecondition(DialoguePrecondition),
    DialogueOutcome(DialogueOutcome),
    ItemType(ItemType),
    WorldCondition(WorldCondition),
    BookType(BookType),
    WeaponType(WeaponType),
    ArmourType(ArmourType),
    U32(u32),
}

#[derive(Default, Debug)]
pub enum BuilderDest {
    #[default]
    None,
    ChoiceAttribute(ChoiceAttribute),
}

impl DataBuilder {
    pub fn build(
        &mut self,
        data: BuilderData,
        dest: BuilderDest,
        cb: fn(BuilderData, BuilderDest, &mut Ui) -> Result<()>,
    ) -> Result<()> {
        self.data = Some(data);
        self.cb = Some(cb);
        self.dest = Some(dest);
        self.state = Some(Default::default());
        self.depth = 0;

        self.choice_menu = ChoiceMenu::new(
            self.choices()?
                .iter()
                .map(|choice| choice.to_string())
                .collect(),
            ChoiceMenuLoc::DataBuilder,
            DataBuilder::choice_cb,
        );

        Ok(())
    }

    /// Iterates the data builder one step deeper.
    /// If the final depth has been reached, returns true.
    fn iterate(&mut self, data: BuilderData) -> Result<()> {
        self.data.as_mut().unwrap().elaborate(data, self.depth)?;
        self.depth += 1;
        Ok(())
    }

    fn choice_cb(s: &str, ui: &mut Ui) -> Result<Option<PopupState>> {
        let data: BuilderData = s.parse()?;
        let next_state = data.state();
        ui.data_builder.iterate(data)?;
        match next_state {
            Some(state) => match state {
                BuilderState::Choice => {
                    ui.data_builder.choice_menu = 
                    ChoiceMenu::new(
                        ui.data_builder
                            .choices()?
                            .iter()
                            .map(|choice| choice.to_string())
                            .collect(),
                        ChoiceMenuLoc::DataBuilder,
                        DataBuilder::choice_cb,
                    );
                }
                BuilderState::Text => ui.data_builder.text_editor = 
                    // TODO: maybe add an appropriate title & default
                    TextEditor::new("", &s.parse::<BuilderData>()?.inner().ok_or(anyhow!("{s} has no inner"))?.to_string(), TextEditorLoc::DataBuilder, DataBuilder::text_cb),
            }
            None => {
                ui.data_builder.state = next_state;
                return Ok(None);
            }
        }
        ui.data_builder.state = next_state;
        Ok(Some(PopupState::DataBuilder))
    }

    fn text_cb(s: &FormattedString, ui: &mut Ui) -> Result<()> {
        ui.data_builder.iterate(s.to_string().parse()?)?;
        Ok(())
    }

    pub fn call(&mut self, ui: &mut Ui) -> Result<()> {
        self.cb
            .take()
            .ok_or(anyhow!("tried to call an inactive data builder"))?(
            self.data
                .take()
                .ok_or(anyhow!("tried to call an inactive data builder"))?,
            self.dest
                .take()
                .ok_or(anyhow!("tried to call an incactive data builder"))?,
            ui,
        )?;
        self.state = None;
        self.depth = 0;
        Ok(())
    }

    pub fn choices(&self) -> Result<Vec<BuilderData>> {
        let data = self
            .data
            .as_ref()
            .ok_or(anyhow!("tried to get choices from inactive data builder"))?;
        Self::choices_internal(data, self.depth)
    }

    fn try_data(&self) -> Result<&BuilderData> {
        self.data
            .as_ref()
            .ok_or_else(|| anyhow!("cannot access data in inactive data builder"))
    }

    // TODO: add test cases instead of relying on the compiler to figure out which cases aren't
    // addressed
    // TODO: make this method use inner();
    fn choices_internal(data: &BuilderData, depth: usize) -> Result<Vec<BuilderData>> {
        let error = Err(anyhow!("descended to depth {depth} in {data}"));
        if depth == 0 {
            match data {
                BuilderData::DialoguePrecondition(_) => Ok(DialoguePrecondition::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                BuilderData::DialogueOutcome(_) => Ok(DialogueOutcome::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                BuilderData::ItemType(_) => Ok(ItemType::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                BuilderData::WorldCondition(_) => Ok(WorldCondition::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                BuilderData::BookType(_) => Ok(BookType::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                BuilderData::WeaponType(_) => Ok(WeaponType::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                BuilderData::ArmourType(_) => Ok(ArmourType::iter()
                    .map(|item| BuilderData::from(item))
                    .collect()),
                _ => error,
            }
        } else {
            match data {
                BuilderData::DialoguePrecondition(inner) => match inner {
                    DialoguePrecondition::InterlocutorHasSpecificItem(_) => error,
                    DialoguePrecondition::InterlocutorHasItem(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    DialoguePrecondition::WorldConditionIsActive(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    DialoguePrecondition::WorldConditionIsInactive(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                },
                BuilderData::DialogueOutcome(inner) => match inner {
                    DialogueOutcome::GiveInterlocutorItem(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    DialogueOutcome::GiveInterlocutorSpecificItem(_) => error,
                    DialogueOutcome::TakeInterlocutorSpecificItem(_) => error,
                    DialogueOutcome::AddWorldCondition(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    DialogueOutcome::RemoveWorldCondition(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                },
                BuilderData::ItemType(inner) => match inner {
                    ItemType::Book(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    ItemType::Weapon(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    ItemType::Armour(data) => {
                        Self::choices_internal(&BuilderData::from(data.clone()), depth - 1)
                    }
                    ItemType::Torch => error,
                },
                _ => error,
            }
        }
    }

    pub fn get_active_screen_mut(&mut self) -> Result<&mut dyn Screen> {
        match self.state.as_ref().ok_or(anyhow!(
            "cannot get active screen from inactive data builder"
        ))? {
            BuilderState::Choice => Ok(&mut self.choice_menu),
            BuilderState::Text => Ok(&mut self.text_editor),
        }
    }

    pub fn get_active_screen(&self) -> Result<&dyn Screen> {
        match self.state.as_ref().ok_or(anyhow!(
            "cannot get active screen from inactive data builder"
        ))? {
            BuilderState::Choice => Ok(&self.choice_menu),
            BuilderState::Text => Ok(&self.text_editor),
        }
    }
}

impl Screen for DataBuilder {
    fn update(&mut self, _world: &World) -> Result<()> {
        Ok(())
    }

    fn render(&mut self, frame: &mut Frame, world: &World, area: Rect) -> Result<()> {
        let b = Block::bordered().title(format!("{}", self.try_data()?));
        let inner_area = centered_rect(area, 1, 1);

        self.get_active_screen_mut()?
            .render(frame, world, inner_area)?;
        frame.render_widget(b, area);
        Ok(())
    }

    fn handle_key_events(
        &mut self,
        key_event: KeyEvent,
        scheme: ControlSchemeType,
        world: &World,
    ) -> Option<UiCallbackPreset> {
        self.get_active_screen_mut()
            .expect("cannot handle key events for inactive data builder")
            .handle_key_events(key_event, scheme, world)
    }

    fn refresh_rate(&self) -> u16 {
        todo!()
    }
}

impl BuilderData {
    // TODO: make this method use inner()
    fn state(&self) -> Option<BuilderState> {
        match self {
            BuilderData::DialoguePrecondition(inner) => match inner {
                DialoguePrecondition::InterlocutorHasSpecificItem(_) => Some(BuilderState::Text),
                DialoguePrecondition::InterlocutorHasItem(_) => Some(BuilderState::Choice),

                DialoguePrecondition::WorldConditionIsActive(_) => Some(BuilderState::Choice),

                DialoguePrecondition::WorldConditionIsInactive(_) => Some(BuilderState::Choice),
            },
            BuilderData::DialogueOutcome(inner) => match inner {
                DialogueOutcome::GiveInterlocutorItem(_) => Some(BuilderState::Choice),

                DialogueOutcome::GiveInterlocutorSpecificItem(_) => Some(BuilderState::Text),
                DialogueOutcome::TakeInterlocutorSpecificItem(_) => Some(BuilderState::Text),
                DialogueOutcome::AddWorldCondition(_) => Some(BuilderState::Choice),

                DialogueOutcome::RemoveWorldCondition(_) => Some(BuilderState::Choice),
            },
            BuilderData::ItemType(inner) => match inner {
                ItemType::Book(_) => Some(BuilderState::Choice),

                ItemType::Weapon(_) => Some(BuilderState::Choice),

                ItemType::Armour(_) => Some(BuilderState::Choice),

                ItemType::Torch => None,
            },
            _ => None,
        }
    }

    fn elaborate(&mut self, data: BuilderData, depth: usize) -> Result<()> {
        if depth == 0 {
            *self = data;
        } else {
            let mut inner = self.inner().ok_or(anyhow!("{self} has no inner value"))?;
            inner.elaborate(data, depth - 1)?;
            self.set_inner(inner)?;
        }
        Ok(())
    }

    fn set_inner(&mut self, value: BuilderData) -> Result<()> {
        let error = Err(anyhow!("cannot set inner value for {self} to {value}"));
        match self {
            BuilderData::DialoguePrecondition(inner) => match inner {
                DialoguePrecondition::InterlocutorHasSpecificItem(inner) => {
                    if let BuilderData::U32(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialoguePrecondition::InterlocutorHasItem(inner) => {
                    if let BuilderData::ItemType(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialoguePrecondition::WorldConditionIsActive(inner) => {
                    if let BuilderData::WorldCondition(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialoguePrecondition::WorldConditionIsInactive(inner) => {
                    if let BuilderData::WorldCondition(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
            }
            BuilderData::DialogueOutcome(inner) => match inner {
                DialogueOutcome::GiveInterlocutorItem(inner) => {
                    if let BuilderData::ItemType(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialogueOutcome::GiveInterlocutorSpecificItem(inner) => {
                    if let BuilderData::U32(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialogueOutcome::TakeInterlocutorSpecificItem(inner) => {
                    if let BuilderData::U32(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialogueOutcome::AddWorldCondition(inner) => {
                    if let BuilderData::WorldCondition(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                DialogueOutcome::RemoveWorldCondition(inner) => {
                    if let BuilderData::WorldCondition(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
            }
            BuilderData::ItemType(inner) => match inner {
                ItemType::Book(inner) => {
                    if let BuilderData::BookType(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                ItemType::Weapon(inner) => {
                    if let BuilderData::WeaponType(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                ItemType::Armour(inner) => {
                    if let BuilderData::ArmourType(value) = value {
                        *inner = value;
                        Ok(())
                    } else {
                        error
                    }
                }
                ItemType::Torch => Err(anyhow!("cannot set inner value for {self}")),
            }
            BuilderData::WorldCondition(inner) => match inner {
                WorldCondition::DiscoveredTimeIsles => Err(anyhow!("cannot set inner value for {self}")),
            }
            _ => Err(anyhow!("cannot set inner value for {self}")),
        }
    }

    fn inner(&self) -> Option<BuilderData> {
        match self {
            BuilderData::DialoguePrecondition(inner) => match inner {
                DialoguePrecondition::InterlocutorHasSpecificItem(inner) => Some(BuilderData::from(inner.clone())),
                DialoguePrecondition::InterlocutorHasItem(inner) => Some(BuilderData::from(inner.clone())),
                DialoguePrecondition::WorldConditionIsActive(inner) => Some(BuilderData::from(inner.clone())),
                DialoguePrecondition::WorldConditionIsInactive(inner) => Some(BuilderData::from(inner.clone())),
            }
            BuilderData::DialogueOutcome(inner) => match inner {
                DialogueOutcome::GiveInterlocutorItem(inner) => Some(BuilderData::from(inner.clone())),
                DialogueOutcome::GiveInterlocutorSpecificItem(inner) => Some(BuilderData::from(inner.clone())),
                DialogueOutcome::TakeInterlocutorSpecificItem(inner) => Some(BuilderData::from(inner.clone())),
                DialogueOutcome::AddWorldCondition(inner) => Some(BuilderData::from(inner.clone())),
                DialogueOutcome::RemoveWorldCondition(inner) => Some(BuilderData::from(inner.clone())),
            }
            BuilderData::ItemType(inner) => match inner {
                ItemType::Book(inner) => Some(BuilderData::from(inner.clone())),
                ItemType::Weapon(inner) => Some(BuilderData::from(inner.clone())),
                ItemType::Armour(inner) => Some(BuilderData::from(inner.clone())),
                ItemType::Torch => None,
            }
            BuilderData::WorldCondition(inner) => match inner {
                WorldCondition::DiscoveredTimeIsles => None,
            }
            _ => None,
        }
    }
}

impl Into<PopupState> for BuilderState {
    fn into(self) -> PopupState {
        match self {
            BuilderState::Choice => PopupState::ChoiceMenu,
            BuilderState::Text => PopupState::TextEditor,
        }
    }
}

// TODO: change to TryFrom
impl From<PopupState> for BuilderState {
    fn from(value: PopupState) -> Self {
        match value {
            PopupState::ChoiceMenu => BuilderState::Choice,
            PopupState::TextEditor => BuilderState::Text,
            PopupState::DataBuilder => panic!("cannot convert popup state {value:?} to builder state"),
        }
    }
}

impl TryInto<DialogueOutcome> for BuilderData {
    type Error = anyhow::Error;

    fn try_into(self) -> std::result::Result<DialogueOutcome, Self::Error> {
        if let BuilderData::DialogueOutcome(outcome) = self {
            Ok(outcome)
        } else {
            Err(anyhow!("cannot convert {self} into a dialogue outcome"))
        }
    }
}

impl From<ChoiceAttribute> for BuilderDest {
    fn from(value: ChoiceAttribute) -> Self {
        BuilderDest::ChoiceAttribute(value)
    }
}

impl From<DialoguePrecondition> for BuilderData {
    fn from(value: DialoguePrecondition) -> Self {
        BuilderData::DialoguePrecondition(value)
    }
}

impl From<DialogueOutcome> for BuilderData {
    fn from(value: DialogueOutcome) -> Self {
        BuilderData::DialogueOutcome(value)
    }
}

impl From<ItemType> for BuilderData {
    fn from(value: ItemType) -> Self {
        BuilderData::ItemType(value)
    }
}

impl From<WorldCondition> for BuilderData {
    fn from(value: WorldCondition) -> Self {
        BuilderData::WorldCondition(value)
    }
}

impl From<BookType> for BuilderData {
    fn from(value: BookType) -> Self {
        BuilderData::BookType(value)
    }
}

impl From<WeaponType> for BuilderData {
    fn from(value: WeaponType) -> Self {
        BuilderData::WeaponType(value)
    }
}

impl From<ArmourType> for BuilderData {
    fn from(value: ArmourType) -> Self {
        BuilderData::ArmourType(value)
    }
}

impl From<u32> for BuilderData {
    fn from(value: u32) -> Self {
        BuilderData::U32(value)
    }
}
