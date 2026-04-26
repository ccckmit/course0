---
name: llm_wiki
description: "Build and maintain a personal wiki using LLMs - a persistent, compounding knowledge base"
license: MIT
metadata:
  author: karpathy
  version: "1.0.0"
  domain: knowledge management
  triggers: wiki, knowledge base, personal knowledge, llm wiki
  source: https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f
---

## Core Idea

Instead of just RAG (retrieve at query time), the LLM **incrementally builds and maintains a persistent wiki** — a structured, interlinked collection of markdown files that sits between you and the raw sources.

- Raw sources: immutable, never modified
- Wiki: LLM-generated markdown files, summaries, entity pages, concept pages
- Schema: instructions telling LLM how to structure and maintain the wiki

## Architecture

```
Raw Sources → LLM reads → Wiki (markdown files) ← Schema (CLAUDE.md/AGENTS.md)
```

**Layers:**
1. **Raw sources** - immutable documents, papers, images
2. **Wiki** - LLM-generated markdown files with summaries, entity/concept pages
3. **Schema** - config file telling LLM conventions and workflows

## Operations

**Ingest:** Drop new source → LLM processes → updates index, entity pages, cross-references

**Query:** Search wiki → synthesize answer → file useful answers back as new pages

**Lint:** Periodically check for contradictions, stale claims, orphan pages, missing links

## Index and Log

- **index.md** - catalog of all wiki pages with summaries, organized by category
- **log.md** - chronological append-only record (ingests, queries, lint passes)

Format log entries: `## [2026-04-24] ingest | Source Title`

## Tools

- Obsidian for editing/browsing
- Obsidian Web Clipper for capturing articles
- qmd for search over markdown (optional)
- Marp for slide decks

## Why It Works

Maintenance is the bottleneck. LLMs don't get bored, can touch 15 files in one pass, and keep the wiki current.

Human job: curate sources, ask questions
LLM job: summarizing, cross-referencing, filing, bookkeeping

## Schema Example (CLAUDE.md/AGENTS.md)

```markdown
# Wiki Schema

## Structure
- entities/ - people, places, concepts
- sources/ - summary pages for each source
- concepts/ - topic summaries
- index.md - catalog
- log.md - timeline

## Workflow
1. Ingest: read source → write summary → update index → update cross-refs
2. Query: read index → find pages → synthesize → file back if valuable
3. Lint: check for orphans, contradictions, stale content
```

## Reference

- Original: https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f