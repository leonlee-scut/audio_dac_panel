---
name: github-library-submodule
description: "Use when the user wants to find a library on GitHub, interactively choose among matching repositories, download it under ThirdParty, and add it to the current main repository as a Git submodule."
argument-hint: "Optional GitHub library name or owner/repository"
---

# Add a GitHub Library as a Submodule

## Goal

Find the GitHub repository for a library, confirm the exact upstream with the user, then add it under `ThirdParty/<repository-name>` as a submodule of the current main repository.

## Procedure

1. **Collect the search term interactively.** If the user has not supplied a library name or `owner/repository`, ask them to enter it. Do not guess a repository from surrounding code or start a download before the user identifies the library.

2. **Search GitHub repositories.** Use the available GitHub repository search integration. If unavailable, use GitHub's repository search page or the GitHub CLI when installed. Search by the user-provided name and collect useful identifying details: canonical `owner/repository`, URL, description, primary language, stars, license, and default branch.

3. **Resolve duplicate or ambiguous matches.** If multiple plausible repositories are found, show a numbered/choice list with the identifying details and ask the user to select one. Do not choose solely by popularity. If one strong match exists, still show its canonical owner/repository and URL and ask for confirmation before fetching it. If the user rejects it, refine the search.

4. **Check the main repository and target path.**
   - Confirm the current workspace is inside the intended main Git repository with `git rev-parse --show-toplevel`. Do not initialize a new repository or add the submodule to an enclosing repository without confirmation.
   - Derive the destination as `ThirdParty/<repository-name>` using the selected repository's basename. Verify the resolved path stays inside the main repository and contains no path traversal.
   - Inspect `.gitmodules`, `git submodule status`, and `git status --short -- <destination>` before changing an existing target.

5. **Handle an existing destination interactively.**
   - If it is already the same repository submodule, report its URL and checked-out commit. Ask whether to keep it or update it; do not add a duplicate.
   - If it is a different submodule or an ordinary directory/file, show the exact path and relevant Git status, then ask whether to replace it or cancel. If it contains modified, staged, or untracked user data, explicitly describe what would be displaced and obtain confirmation before moving or removing anything. Never silently overwrite or discard user work. If replacement cannot be performed safely, stop and explain why.
   - If the user declines replacement, make no filesystem or Git changes.

6. **Confirm the upstream and add the submodule.** Before mutation, confirm the selected canonical URL and destination. Use the repository's default branch unless the user specifies a branch or tag. Create `ThirdParty` if needed, then run the equivalent of:

   ```powershell
   git submodule add -- <canonical-url> ThirdParty/<repository-name>
   ```

   If the user selected a branch, pass it with `git submodule add --branch <branch>`. For a tag or exact commit, add the submodule and check out that ref inside its directory. Do not embed credentials in the URL or print tokens.

7. **Verify the result.** Confirm that:
   - the destination contains the cloned repository and `git -C <destination> rev-parse HEAD` succeeds;
   - `.gitmodules` contains the selected URL and destination path;
   - `git submodule status` reports the new submodule;
   - `git status --short` shows only the expected `.gitmodules` and submodule changes, plus any previously disclosed replacement effects.

8. **Stop before committing.** `git submodule add` stages submodule metadata and the gitlink. Do not create a commit unless explicitly requested. Do not revert or clean unrelated worktree changes.

## Interaction and Safety Rules

- Ask one question at a time when a user decision is required: library name, repository choice, upstream confirmation, and overwrite decision.
- When presenting search results, distinguish similarly named repositories by owner and canonical URL.
- If GitHub search returns no credible match, ask for more detail (owner, language, purpose, or URL) rather than installing an unrelated repository.
- If network access, GitHub authentication, or Git operations fail, report the exact blocker and leave the repository in a recoverable state. Never claim installation succeeded without verification.
- Keep the operation limited to the selected submodule and its required `.gitmodules`/index entries.
