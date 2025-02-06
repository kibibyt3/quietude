use std::{
    fs::remove_file, path::{Path, PathBuf},
};

use anyhow::{anyhow, Result};
use log::trace;
use parse_display::{Display, FromStr};
use serde::{Deserialize, Serialize};
use strum::EnumIter;

use crate::{
    constants::{DIALOGUE_DIR_NAME, SAVE_EXTENSION},
    store::{load, load_profile, save},
    types::{Color, FormattedString},
};

use super::{conditions::WorldCondition, item::ItemType, log::StringStyle, world::World};

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct DialogueTree {
    nodes: Vec<DialogueNode>,
    active_node_name: String,
    pub speaker_name: String,
    pub speaker_id: u32,
    pub interlocutor_id: u32,
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct DialogueNode {
    name: String,
    speaker_dialogue: FormattedString,
    choices: Vec<DialogueChoice>,
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub struct DialogueChoice {
    text: FormattedString,
    preconditions: Vec<DialoguePrecondition>,
    outcomes: Vec<DialogueOutcome>,
    destination: String,
}

#[derive(FromStr, Display, Debug, EnumIter, Clone, Serialize, Deserialize, PartialEq)]
#[display("{}({0})")]
pub enum DialoguePrecondition {
    InterlocutorHasSpecificItem(u32),
    InterlocutorHasItem(ItemType),
    WorldConditionIsActive(WorldCondition),
    WorldConditionIsInactive(WorldCondition),
}

#[derive(FromStr, Display, EnumIter, Clone, Debug, Serialize, Deserialize, PartialEq)]
#[display("{}({0})")]
pub enum DialogueOutcome {
    GiveInterlocutorItem(ItemType),
    GiveInterlocutorSpecificItem(u32),
    TakeInterlocutorSpecificItem(u32),
    AddWorldCondition(WorldCondition),
    RemoveWorldCondition(WorldCondition),
}

impl DialogueTree {
    pub fn new(speaker_name: &str) -> DialogueTree {
        DialogueTree {
            nodes: vec![DialogueNode::new("start")],
            active_node_name: String::from("start"),
            speaker_name: speaker_name.to_string(),
            speaker_id: 0,
            interlocutor_id: 0,
        }
    }

    pub fn save(&self, path: &Path) -> Result<()> {
        trace!("saving dialogue tree to {}", &Self::path_from(path, &self.speaker_name).to_str().unwrap());
        if self.speaker_name == "" {
            return Err(anyhow!("tried to save a dialogue tree without a speaker name"));
        }
        save(
            &Self::path_from(path, &self.speaker_name),
            &(&self.speaker_name, &self.nodes),
        )
    }

    // TODO: make this API consistently use either u32 arguments or names.
    pub fn from_entity_name(speaker_id: u32, interlocutor_id: u32) -> Result<Self> {
        let (speaker_name, nodes) = load_profile(Path::new(&format!("{speaker_id}.json")))?;
        Ok(DialogueTree {
            speaker_name,
            nodes,
            active_node_name: String::from("start"),
            speaker_id,
            interlocutor_id,
        })
    }

    pub fn load_or_init(name: &str, path: &Path) -> Result<DialogueTree> {
        trace!("loading or initializing dialogue tree from path {}", path.to_str().unwrap());
        let complete_path = Self::path_from(path, name);
        if let Ok((speaker_name, nodes)) = load(&complete_path) {
            Ok(DialogueTree {
                speaker_name,
                nodes,
                active_node_name: String::from("start"),
                speaker_id: 0, // FIXME
                interlocutor_id: 0,
            })
        } else {
            let tree = DialogueTree::new(name);
            trace!("saving dialogue tree to path {}", path.to_string_lossy());
            tree.save(&path)?;
            Ok(tree)
        }
    }

    pub fn get_active_node(&self) -> Result<&DialogueNode> {
        for node in &self.nodes {
            if node.name == self.active_node_name {
                return Ok(node);
            }
        }

        Err(anyhow!("active node {} not found", self.active_node_name))
    }
    
    pub fn get_active_node_mut(&mut self) -> Result<&mut DialogueNode> {
        for node in &mut self.nodes {
            if node.name == self.active_node_name {
                return Ok(node);
            }
        }

        Err(anyhow!("active node {} not found", self.active_node_name))
    }

    pub fn get_outcomes_and_destination_from_choice(
        &self,
        choice: usize,
        world: &World,
    ) -> Result<(Vec<DialogueOutcome>, String)> {
        Ok(self
            .get_active_node()?
            .get_outcomes_and_destination_from_choice(choice, world)?)
    }

    pub fn make_choice(&mut self, destination: &str) {
        self.active_node_name = String::from(destination);
    }

    pub fn delete(name: &str, path: &Path) -> Result<()> {
        let path = Self::path_from(path, name);
        remove_file(path)?;
        Ok(())
    }

    pub fn exists(name: &str, path: &Path) -> bool {
        Self::path_from(path, name).exists()
    }

    pub fn path_from(path: &Path, name: &str) -> PathBuf {
        let mut path = path.to_path_buf();
        path.push(DIALOGUE_DIR_NAME);
        path.push(format!("{name}"));
        path.set_extension(SAVE_EXTENSION[1..].to_string());
        path
    }

}

impl DialogueNode {
    pub fn new(name: &str) -> DialogueNode {
        DialogueNode {
            name: String::from(name),
            speaker_dialogue: FormattedString::raw(&None, ""),
            choices: vec![],
        }
    }

    pub fn choices_text(&self, world: &World) -> Result<Vec<FormattedString>> {
        let choices = self.choices(world)?;
        let text = choices
            .iter()
            .map(|choice| choice.text.clone())
            .collect::<Vec<_>>();
        Ok(text)
    }

    pub fn choices_text_unconditional(&self) -> Result<Vec<FormattedString>> {
        let choices = self.choices_unconditional();
        let text = choices
            .iter()
            .map(|choice| choice.text.clone())
            .collect::<Vec<_>>();
        Ok(text)
    }

    pub fn choices_count_unconditional(&self) -> usize {
        self.choices.len()
    }

    fn choices(&self, world: &World) -> Result<Vec<DialogueChoice>> {
        let mut choices = Vec::new();
        for choice in &self.choices {
            if choice.preconditions_are_met(world)? {
                choices.push(choice.clone());
            }
        }

        Ok(choices)
    }

    fn choices_unconditional(&self) -> &Vec<DialogueChoice> {
        &self.choices
    }

    pub fn set_choice_text(&mut self, index: usize, s: FormattedString) {
        if index < self.choices.len() {
            self.choices[index].text = s;
        } else {
            self.choices.push(DialogueChoice::new(s));
        }
    }

    pub fn choice_text(&self, index: usize) -> FormattedString {
        if index < self.choices.len() {
            self.choices[index].text.clone()
        } else {
            FormattedString::raw(&None, "")
        }
    }

    pub fn set_choice_destination(&mut self, index: usize, s: &str) {
        self.choices[index].destination = s.to_string();
    }
    
    pub fn choice_destination(&self, index: usize) -> String {
        self.choices[index].destination.clone()
    }
    
    pub fn push_choice_precondition(&mut self, index: usize, precondition: DialoguePrecondition) {
        self.choices[index].preconditions.push(precondition.clone());
    }

    pub fn remove_choice_precondition(&mut self, choice_index: usize, precondition_index: usize) {
        self.choices[choice_index].preconditions.remove(precondition_index);
    }

    pub fn choice_preconditions(&self, index: usize) -> Vec<DialoguePrecondition> {
        self.choices[index].preconditions.clone()
    }

    pub fn add_choice_outcome(&mut self, index: usize, outcome: &DialogueOutcome) {
        self.choices[index].outcomes.push(outcome.clone());
    }

    pub fn remove_choice_outcome(&mut self, choice_index: usize, outcome_index: usize) {
        self.choices[choice_index].outcomes.remove(outcome_index);
    }

    pub fn choice_outcomes(&self, index: usize) -> Vec<DialogueOutcome> {
        self.choices[index].outcomes.clone()
    }
    
    pub fn set_speaker_dialogue(&mut self, s: &FormattedString) {
        self.speaker_dialogue = s.clone();
    }

    pub fn speaker_dialogue(&self) -> &FormattedString {
        &self.speaker_dialogue
    }

    fn get_outcomes_and_destination_from_choice(
        &self,
        index: usize,
        world: &World,
    ) -> Result<(Vec<DialogueOutcome>, String)> {
        let choices = self.choices(world)?;
        if index >= choices.len() {
            return Err(anyhow!(
                "could not find choice at index {index} in dialogue node"
            ));
        }
        Ok((
            choices[index].outcomes.clone(),
            choices[index].destination.clone(),
        ))
    }
}

impl DialogueChoice {
    pub fn new(s: FormattedString) -> Self {
        DialogueChoice {
            text: s,
            preconditions: vec![],
            outcomes: vec![],
            destination: "start".to_string(),
        }
    }

    pub fn preconditions_are_met(&self, world: &World) -> Result<bool> {
        for condition in &self.preconditions {
            if !condition.is_met(world)? {
                return Ok(false);
            }
        }

        Ok(true)
    }
}

impl DialoguePrecondition {
    fn is_met(&self, world: &World) -> Result<bool> {
        let interlocutor_id = world.dialogue_tree.as_ref().unwrap().interlocutor_id;
        let speaker_id = world.dialogue_tree.as_ref().unwrap().speaker_id;
        let outcome = match self {
            DialoguePrecondition::InterlocutorHasItem(item) => world
                .active_chunk
                .get_entity_from_id(interlocutor_id)
                .ok_or(anyhow!("interlocutor {} not found", interlocutor_id))?
                .has_item(item.clone()),
            DialoguePrecondition::InterlocutorHasSpecificItem(id) => world
                .active_chunk
                .get_entity_from_id(speaker_id)
                .ok_or(anyhow!("interlocutor {} not found", speaker_id))?
                .has_specific_item(*id),
            DialoguePrecondition::WorldConditionIsActive(condition) => {
                world.has_condition(*condition)
            }
            DialoguePrecondition::WorldConditionIsInactive(condition) => {
                !world.has_condition(*condition)
            }
        };

        Ok(outcome)
    }
}

impl Default for DialoguePrecondition {
    fn default() -> Self {
        DialoguePrecondition::InterlocutorHasSpecificItem(Default::default())
    }
}

/*
impl Display for DialogueOutcome {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let s = match self {
            DialogueOutcome::GiveInterlocutorItem(_) => "Give Interlocutor Item",
            DialogueOutcome::GiveInterlocutorSpecificItem(_) => "Give Interlocutor Specific Item",
            DialogueOutcome::TakeInterlocutorSpecificItem(_) => "Take Interlocutor's Specific Item",
            DialogueOutcome::AddWorldCondition(_) => "Add World Condition",
            DialogueOutcome::RemoveWorldCondition(_) => "Remove World Condition",
        };
        write!(f, "{s}")
    }
}
*/

impl Default for DialogueOutcome {
    fn default() -> Self {
        DialogueOutcome::GiveInterlocutorSpecificItem(Default::default())
    }
}
