# Contributing to libgd

:+1::tada: First off, thanks for taking the time to contribute! :tada::+1:

#### Table Of Contents

[What should I know before I get started?](#what-should-i-know-before-i-get-started)
  * [Code of Conduct](#code-of-conduct)
  * [Repositories](#Repositories)

[How Can I Contribute?](#how-can-i-contribute)
  * [Documentations and examples](#documentation-and-examples)
  * [Reporting Bugs or suggest enhancements](#reporting-bugs-or-request-a-feature)
  * [Pull Requests](#pull-requests)

[Styleguides](#styleguides)
  * [Git Commit Messages](#git-commit-messages)
  * [Coding standard](#coding-standard)
  * [Documentation Styleguide](#documentation-styleguide)

## What should I know before I get started?

### Code of Conduct

This project adheres to the Contributor Covenant [code of conduct](CODE_OF_CONDUCT.md).
By participating, you are expected to uphold this code.
Please report unacceptable behavior according to the [enforcement section](CODE_OF_CONDUCT.md#enforcement).

### Repositories

* [The main libGD repository](https://github.com/libgd/libgd)
  for the library and utilities released as part of libgd
* [The libgd website repository](https://github.com/libgd/libgd.github.io)
  for the libgd.org website


## How Can I Contribute?

### Documentation and examples

We have a relatively complete API documentation.
However good documentation requires more than API docs.
If you feel like writing tutorials, examples (C or other language binding)
or add content to the documentation, you will be more than welcome.

We use NaturalDocs 1.5x for the API and general documentations.
It supports rest, markdown or text
(see the docs folder in our repository for examples).

### Reporting Bugs or request a feature

This section guides you through submitting a bug or feature report for LibGD.
Following these guidelines helps maintainers and the community understand your report :pencil:,
reproduce the behavior :computer: :computer:, and find related reports :mag_right:.

Before creating bug reports, please check [this list](#before-submitting-a-bug-report)
as you might find out that you don't need to create one.
When you are creating a bug report,
please [include as many details as possible](#how-do-i-submit-a-good-bug-report).
If you'd like, you can use [this template](#template-for-submitting-bug-reports)
to structure the information.


### Before Submitting A Bug Report

* **Perform a [cursory search](https://github.com/issues?q=+is%3Aissue+user%3Alibgd)**
  to see if the problem has already been reported.
  If it has, add a comment to the existing issue instead of opening a new one.


### Before Submitting A feature Request

Please take the time to clearly describe what this feature should achieve.
A good specification will help us to implement it, if it fits the roadmap.

We also have some open features,
please check if the feature you would like to have is not already there.
Maybe the specification can boost its implementation?

### How Do I Submit A (Good) Bug Report?

For security related bugs, see [SECURITY.md](SECURITY.md).

Bugs are tracked as [GitHub issues](https://guides.github.com/features/issues/).
After you've determined [which repository](#repositories) your bug is related to,
create an issue on that repository and provide the following information.

Explain the problem and include additional details
to help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem**
  in as many details as possible.
* **Provide specific examples to demonstrate the steps**.
  Include links to files or GitHub projects, or copy/pasteable workable example
  (for example see [this simple test case](https://github.com/libgd/libgd/blob/master/tests/gd/gd_null.c),
  which you use in those examples.
  If you're providing snippets in the issue,
  use [Markdown code blocks](https://help.github.com/articles/markdown-basics/#multiple-lines).
* **Describe the behavior you observed after following the steps**
  and point out what exactly is the problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **Include images, expected and result images** which show you following
  the described steps and clearly demonstrate the problem.
* **If you're reporting that libGD crashed**,
  include a crash report with a stack trace from the operating system.
  Include the crash report in the issue in a
  [code block](https://help.github.com/articles/markdown-basics/#multiple-lines), a
  [file attachment](https://help.github.com/articles/file-attachments-on-issues-and-pull-requests/),
  or put it in a [gist](https://gist.github.com/) and provide link to that gist.


This text has been largely inspired by the Atom contributing notes,
which are amazingly complete.

## Styleguides

These sections tend to bore people. But if you don't follow them, it slows
down reviews from developers (and tends to tire/annoy them). Please give it
a look over and feel free to ask questions.

### Git Commit Messages

Rather than duplicate a ton of great explanations, please read this site:
http://chris.beams.io/posts/git-commit/

Yes, it's long, but once you read and understand it once, it applies to
every git project out there, not just GD.

In case that site is broken, here is an archived link:
http://web.archive.org/web/20160706012209/http://chris.beams.io/posts/git-commit/

### Coding standard

#### Style

First, coding style should be consistent. If you're modifying a file that
might not follow this section, stick with that file's style. If we want to
fix the style later, we can.

As for the rest, TBD!

#### Practices

Check for invalid settings (where reasonable) and return an error directly.
Do not use `assert` and do not require the user to follow the documented API.
e.g. Check for `NULL` pointers, and check enum values are within range. The
GD library is used in a lot of hostile environments (like websites) and is
given hostile (user controlled) data. When GD mishandles memory, that can be
used to attack servers. When GD crashes (or calls `assert`), that can take
out a web session.

#### Testing

We love unit tests. Every bugfix, new API, etc... should include a test update
to make sure future work doesn't break things or regress. We want to fix bugs
just once, not over and over again.

Note that we say "should". While we really like tests, we understand that not
all changes are trivial to verify.

Some general guidelines:

* Tests should be standalone: One test should test one thing only.
* Tests should be reproducible: Avoid relying on system settings like /etc.
* Tests must be fast: A good guideline is that they should take <1 second,
  and must take <5 seconds.
* Tests must be documented: Just add a comment block to the top of the file
  with a short description and any existing bugs/URLs.

#### CI

If we are lucky enough to get pull request (PR) from you,
we will do our best to support your efforts to get it in.
As part of this, we setup a CI using github actions, which will be executed on new PR.
Please check out the results of your PR to see if anything went wrong
(it has Linux, macOS and Windows OSes support).
The CI must be green for a PR to be considered.

See [the github PR documentation]{https://docs.github.com/en/github/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request}
for more details about how to create a pull request.

### Documentation Styleguide

Do everything in markdown. It's readable even in plain text, and converts
well into other formats (e.g. HTML). The question is which flavor!

For files that end in `.md`, use
[GitHub markdown](https://guides.github.com/features/mastering-markdown/).

For API documentation (embedded in the code), we use
[Natural Docs](http://www.naturaldocs.org/). You can find more info about
it under the [docs/naturaldocs/](docs/naturaldocs/) subdirectory.

Try to keep the file linewrapped to 80 cols. Sometimes you won't be able to
because of long strings (like links), and that's OK.
