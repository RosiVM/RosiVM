---
layout: post
title: Compiler Architecture
permalink: /compiler-architecture/
---

The compiler has manually written, C++, recursive ascent parser for front-end. And uses [LLVM](https://llvm.org) as compiler back-end.

```
Lexer -> Parser -> Binder -> TypeChecker -> LLVMEmitter
```

Early versions of the language had their grammar defined in BNF and tested for ambiguity using compiler generators. The goal for the lexer, parser, binder and LLVMEmitter is to provide linear O(n) parsing. The TypeChecker comlpexity will depend on the language features. But overall the design is done with compilation performance in mind.

RosiVM tries to get close to C++ runtime performance, but to maintain good compilation times and developer productivity. C-like structures and functions should come with zero costs abstractions, but C++ like classes will generate rtti and TBD - sort of GC or smart pointers.

## Lexer
The lexer processes the souce file as string, character by character, splitting it into tokens. During the tokenization phase the lexer also performs character escaping and number parsing for string and numeric literals.

The lexer is implemented as adaptor on C++ std::string exposing iterator as input rvm::Token stream. The Token keeps start and end line and column information, value assigned by the lexer and std::string::Itterator to the start and end of the token in the underlying source file std::string.

As a side goal for the lexer is to let it work with input iterator over character stream, that would allow it to work with filestreams to implement style linters or formatters (to HTML conversion) without storing the whole file in memory. That's not the case with the rest of the passes.

## Abstract Syntax Tree
The abstract syntax tree is expressed using a group of classes in the rvm::ast namespace. There are tree nodes for module members, type annotations, statements, etc. This is a data structure, the AST tree is created by the Parser and can be itterated using visitors implemented by the Binder, TypeChecker, LLVMEmitter.

## Parser
The recursive ascent parser, given a token input stream from the Lexer, will produce an AST.

Linting and formatting based on the AST can use more contextual information compared with these using just the Lexer. For example such processing can distinguish between identifiers in class names and identifiers for locals.

## Binder
The binder phase of the compilation will itterate all ASTs for each source file and build a table of global symbols. This table will be used to identify name collisions and to lookup global functions, classes etc. during the type checking phase.

## TypeSystem
Each type is expressed by classes in the type system. The type system provides ultilites to check if one type can be assigned to another, to check if a type is invokable, assists in resolving overloads etc.

## TypeChecker
The goal of the type checker is to resolve types for AST nodes and check for type errors.

One of the goalds for the type checker is to be lazy. That is to be able to resolve type for AST node by resolving types for as few AST nodes as possible. When the program is compiled, the compiler will run the TypeChecker on each AST node in the program. But when it is used by language services, the lazy typechecking should provide for fast hints, lookups, suggestions and auto completion.

## LLVMEmitter
The LLVMEmitter itterates AST typechecked in advance and produces LLIR using LLVM's C++ API. LLIR is intermediate presentation used by the LLVM compiler. The IR can then be processed using LLVM's optimization passes and produce a statically linked executable or library.

