use std::{collections::HashMap, hash::Hash};

pub trait UnsortedEq {
    fn unsorted_eq(self, other: Self) -> bool;
}

impl<T, U> UnsortedEq for T
where
    U: Eq + Hash,
    T: IntoIterator<Item = U>,
{
    fn unsorted_eq(self, other: Self) -> bool {
        let lookup = |iter: Self| -> HashMap<U, usize> {
            let mut lookup = HashMap::new();
            for value in iter {
                lookup
                    .entry(value)
                    .and_modify(|counter| *counter += 1)
                    .or_insert(1);
            }
            lookup
        };

        lookup(self) == lookup(other)
    }
}

#[cfg(test)]
mod tests {
    use crate::utils::UnsortedEq;

    #[test]
    fn unsorted_eq() {
        let i1 = vec![1, 7, 3];
        let i2 = vec![7, 3, 1];
        assert!((&i1).unsorted_eq(&i2));

        let i3 = vec![1, 7];
        assert!(!(&i1).unsorted_eq(&i3));
    }
}
