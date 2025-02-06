use quietude::types::FormattedString;
use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize, Debug, PartialEq)]
pub enum Message {
    Popup(FormattedString),
    Log(FormattedString),
}
