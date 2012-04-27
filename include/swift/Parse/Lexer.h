//===--- Lexer.h - Swift Language Lexer -------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Lexer interface.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_LEXER_H
#define SWIFT_LEXER_H

#include "Token.h"

namespace llvm {
  class SourceMgr;
}

namespace swift {
  class DiagnosticEngine;
  class Identifier;
  class InFlightDiagnostic;
  class ASTContext;
  
  template<typename ...T> struct Diag;

class Lexer {
  llvm::SourceMgr &SourceMgr;
  DiagnosticEngine *Diags;
  
  const char *BufferStart;
  const char *BufferEnd;
  const char *CurPtr;

  Token NextToken;
  
  Lexer(const Lexer&) = delete;
  void operator=(const Lexer&) = delete;
public:
  Lexer(llvm::StringRef Buffer, llvm::SourceMgr &SourceMgr,
        DiagnosticEngine *Diags)
    : Lexer(Buffer, SourceMgr, Diags, Buffer.begin()) { }

  Lexer(llvm::StringRef Buffer, llvm::SourceMgr &SourceMgr,
        DiagnosticEngine *Diags, 
        const char *CurrentPosition);

  void lex(Token &Result) {
    Result = NextToken;
    if (Result.isNot(tok::eof))
      lexImpl();
  }

  /// peekNextToken - Return the next token to be returned by Lex without
  /// actually lexing it.
  const Token &peekNextToken() const { return NextToken; }

  /// \brief Retrieve the source location that points just past the
  /// end of the token refered to by \c Loc.
  ///
  /// \param SM The source manager in which the given source location
  /// resides.
  ///
  /// \param Loc The source location of the beginning of a token.
  static SourceLoc getLocForEndOfToken(llvm::SourceMgr &SM, SourceLoc Loc);

  /// \brief Determines if the given string is a valid non-operator
  /// identifier.
  static bool isIdentifier(llvm::StringRef identifier);

  SourceLoc getLocForStartOfBuffer() const {
    return SourceLoc(llvm::SMLoc::getFromPointer(BufferStart));
  }
  
  /// getEncodedStringLiteral - Given a string literal token, return the bytes
  /// that the actual string literal should codegen to.  If a copy needs to be
  /// made, it will be allocated out of the ASTContext allocator.
  static StringRef getEncodedStringLiteral(const Token &Str, ASTContext &Ctx); 
  
private:
  static SourceLoc getSourceLoc(const char *Loc) {
    return SourceLoc(llvm::SMLoc::getFromPointer(Loc));
  }
  InFlightDiagnostic diagnose(const char *Loc, Diag<> ID);
  void lexImpl();
  void formToken(tok Kind, const char *TokStart);
  
  void skipSlashSlashComment();
  void skipSlashStarComment();
  void lexIdentifier();
  void lexDollarIdent();
  void lexOperatorIdentifier();
  void lexNumber();
  
  unsigned lexCharacter(bool StopAtDoubleQuote);
  void lexCharacterLiteral();
  void lexStringLiteral();
  bool isPrecededBySpace();
};
  
  
} // end namespace swift

#endif
