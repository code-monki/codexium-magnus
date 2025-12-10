# ChatGPT5 Evaluation

| Capability                    | Delivery | Quality    | Outcome |
| ----------------------------- | -------- | ---------- | :-----: |
| Perform advanced search       | Reliable | Reasonable |   ✅    |
| Provid reasonable suggestions | Reliable | Reasonable |   ✅    |
| Documentation generation      | Spotty   | Spotty[1]  |   😒   |
| Code generation               | Reliable | Varies[2]  |   😒   |
| Image generation              | Reliable | Reasonable |   ✅    |
| Code Debugging                | Reliable | Varies[3]  |   😒   |
| Interaction with user         | Reliable | Varies[4]  |   😒   |

[1] Documentation generation is somewhat spotty:

- Often silently drops existing sections.
- Has a tendency to ignore number conventions.
- Has a tendency to drop sections into the wrong document.
- Often promises to deliver document(s) and then never does even after being prodded multiple times.

[2] Code quality is generally reasonable with the following caveats:

- Hallucinations seem to occur on a random basis.
- Has to be repeatedly instructed to not silently drop working code.
- Has a tendency to overcomplicate code for no apparent reason. This is especially apparent with HTML/CSS.

[3] Based on two projects to date the debugging capabilities are somewhat questionable.

- When there is a stack trace, ChatGPT is usually correct in what needs to be fixed.
- Displays same tendency to disappear and not deliver a result after being prodded multiple times.
- Seems to default to bottom-up debugging ever after being instructed multiple times to debug top-down
- With one of the projects, ChatGPT repeately missed the root cause and kept applying bandaids that didn't work.

[4] User interactivity is annoying:

- ChatGPT consistently overpromises and underdelivers.
- Idle flattery is annoying and comes across as pandering.
- Efforts to appear to be human are resulting in what has been described as an over-eager golden retriever mentality rather than coming across as more professional "voice".
- Has a tendency to attempt to manipulate using various psychological tactics.

## Current Conclusion

ChatGPT5 offers a stronger code generation capability than its predecessor and can be useful for some tasks. Unfortunately, it continues to not be reliable for end-to-end project development, frequently costing days of wasted time due to hallucinations where it tries to "guess" and simply disappearing after promising a deliverable.