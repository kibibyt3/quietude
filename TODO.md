Finish fleshing out the qattr module so work can resume on qwalk, whose logic
depends heavily on qattr.
- This job means fleshing out Qdatameta access and related functions because pointers will be used!
---
Also, add a member to Qdatameta\_t to reflect the
species of the data contained therein. This will be for error-checking and the
new member's type will be a newly-typedeffed enum that contains all possible
forms the datameta may take.	
