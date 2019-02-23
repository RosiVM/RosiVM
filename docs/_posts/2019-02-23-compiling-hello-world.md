---
layout: post
title: To Compile a Hello World or Not?
permalink: /to-compile-a-hello-world-or-not/
---

I talked with a friend of mine about the RosiVM project. Working on it in my spare time is not delivering fast results. You see, thowing 100 lines of code every one in a while, does not get you quickly to a compiler that can showcase "Hello World".

## 20/80

You know Pareto principle?

> 80% of the effects come from 20% of the causes

Building a compiler does not feel like that. You will be surprised how much of the compiler you must have in place before you can compile and run a simple hello world program.

### Hello World Source Code

Consider the following simple RosiVM program:

```
function main(): int {
    console.log("Hello World!");
    return 0;
}
```

### You Need the Lexer
First you need a big part of your lexer to split the code string into tokens:

```
keyword "function", identifier "main", left-paren, right-paren, colon, identifier "float", left brace...
```

### You Need the Parser
Second you need a big part of your parser to build an AST:

```
function "main"
    proto
        args
        returnTypeAnnotation
            int
    statement-block
        call
            member-access
                identifier "console"
                identifier "log"
            args
                string-literal "Hello World"
        return
            int-literal 0
```

### You Need the Type Checker

Third you need your type checker:

- To figure out the main signature.
- To assert that you are calling the logging function with a string.
- To assert that you are returning a type that can be assigned to the main function return type.

### You Need the Type System

This drags in the implementation of an error reporting and a type system. The actual arguments provided to the function at the call site must be assignable to the formal function arguments of the function declaration, so the implementation needs to cover some primitive types, function signature types, and assignability.

- **You are here**. I mean the RosiVM compiler is here.

The type checker will assign types to the AST nodes if everything is fine, if it is not, it has to log errors.

### You Need Primitive Types

Under the hood the compiler has to also provide implementation of a string. If the language is managed and the string is not copied by value but by reference, you will need to implement the garbage collector or reference counting utilities.

### You Need a Back-end

All that is just a compiler front-end. If you are building a custom language you will probably want to integrate it with an existing compiler back-end. This will save you from going down to assembly level.

Now that the compiler has the AST with the assigned types, it has to convert it to an action tree, that would be LLIR.

```
@.str = private unnamed_addr constant [13 x i8] c"hello world\0A\00"
declare i32 @puts(i8* nocapture) nounwind
define i32 @main() {   ; i32()*
  %cast210 = getelementptr [13 x i8], [13 x i8]* @.str, i64 0, i64 0
  call i32 @puts(i8* %cast210)
  ret i32 0
}
!0 = !{i32 42, null, !"string"}
!foo = !{!0}
```

### You Need C Interop

Finally - output the string to the standard output. Whether it will be an intrinsic function (like converting console.log to call to the C "puts" function in the example above) or ability to link and call C libraries, you need to take that hello world string to the other side. When LLVM is used as back-end it makes perfect sense to have zero cost interop with C. Any modern language will also have the ability to output WebAssembly but that's a story for some other day.

## With That in Mind
You will not see a working "Hello World" example using RosiVM soon. It will require more lines of code in the compiler. But once you see the compiler, it will probably have a sufficient C interop support to load strings from the file system, to spin a run loop, to open a window and draw some text in it, to become self-hosted. Instead of aiming at delivering a "Hello World" working PoC asap, the "road map" will aim at implementing a decent string and interop with C libraries.

Building a compiler is hell lot of a fun, and is all about transforming one tree into another.
