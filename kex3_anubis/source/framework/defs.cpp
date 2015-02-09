//
// Copyright(C) 2014-2015 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Definition system
//

#include "kexlib.h"

//-----------------------------------------------------------------------------
//
// kexDefManager
//
//-----------------------------------------------------------------------------

//
// kexDefManager::~kexDefManager
//

kexDefManager::~kexDefManager(void)
{
}

//
// kexDefManager::Parse
//

void kexDefManager::Parse(kexLexer *lexer)
{
    kexDict *defEntry;
    kexStr key;
    kexStr val;

    while(lexer->CheckState())
    {
        lexer->Find();

        switch(lexer->TokenType())
        {
        case TK_EOF:
            return;
        case TK_IDENIFIER:
            defEntry = defs.Add(lexer->Token());

            lexer->ExpectNextToken(TK_LBRACK);
            while(1)
            {
                lexer->Find();
                if(lexer->TokenType() == TK_RBRACK || lexer->TokenType() == TK_EOF)
                {
                    break;
                }

                key = lexer->Token();

                lexer->Find();
                if(lexer->TokenType() == TK_RBRACK || lexer->TokenType() == TK_EOF)
                {
                    break;
                }

                val = lexer->Token();

                defEntry->Add(key.c_str(), val.c_str());
            }
            break;
        default:
            break;
        }
    }
}

//
// kexDefManager::LoadFilesInDirectory
//

void kexDefManager::LoadFilesInDirectory(const char *directory)
{
    kexStrList list;

    kex::cPakFiles->GetMatchingFiles(list, directory);

    for(unsigned int i = 0; i < list.Length(); ++i)
    {
        kexLexer *lexer;

        // must be a valid text file
        if(list[i].IndexOf(".txt\0") == -1)
        {
            continue;
        }

        if(!(lexer = kex::cParser->Open(list[i].c_str())))
        {
            continue;
        }

        Parse(lexer);

        // we're done with the file
        kex::cParser->Close();
    }
}

//
// kexDefManager::LoadFile
//

void kexDefManager::LoadFile(const char *defFile)
{
    kexLexer *lexer;
    
    if(!(lexer = kex::cParser->Open(defFile)))
    {
        return;
    }
    
    Parse(lexer);
    
    // we're done with the file
    kex::cParser->Close();
}

//
// kexDefManager::GetEntry
//

kexDict *kexDefManager::GetEntry(const char *name)
{
    return defs.Find(name);
}

//-----------------------------------------------------------------------------
//
// kexIndexDefManager
//
//-----------------------------------------------------------------------------

//
// kexIndexDefManager::~kexIndexDefManager
//

kexIndexDefManager::~kexIndexDefManager(void)
{
}

//
// kexIndexDefManager::Parse
//

void kexIndexDefManager::Parse(kexLexer *lexer)
{
    kexDict *defEntry;
    kexDict *defNameEntry;
    kexStr key;
    kexStr val;
    kexStr defName;
    int defIndex;
    
    while(lexer->CheckState())
    {
        lexer->Find();
        
        switch(lexer->TokenType())
        {
            case TK_EOF:
                return;
            case TK_IDENIFIER:
                defName = lexer->Token();
                defIndex = lexer->GetNumber();
                defEntry = defs.Add(defName, defIndex);
                defNameEntry = defs.Add(defName);
                
                lexer->ExpectNextToken(TK_LBRACK);
                while(1)
                {
                    lexer->Find();
                    if(lexer->TokenType() == TK_RBRACK || lexer->TokenType() == TK_EOF)
                    {
                        break;
                    }
                    
                    key = lexer->Token();
                    
                    lexer->Find();
                    if(lexer->TokenType() == TK_RBRACK || lexer->TokenType() == TK_EOF)
                    {
                        break;
                    }
                    
                    val = lexer->Token();
                    
                    defEntry->Add(key.c_str(), val.c_str());
                    defNameEntry->Add(key.c_str(), val.c_str());
                }
                break;
            default:
                break;
        }
    }
}

//
// kexIndexDefManager::GetEntry
//

kexDict *kexIndexDefManager::GetEntry(const int index)
{
    return defs.Find(index);
}