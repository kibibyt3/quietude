use parse_display::{Display, FromStr};
use serde::{Deserialize, Serialize};
use strum::EnumIter;

#[derive(FromStr, Display, Default, EnumIter, Clone, Copy, Debug, Serialize, Deserialize, PartialEq)]
pub enum WorldCondition {
    #[default]
    DiscoveredTimeIsles,
}
