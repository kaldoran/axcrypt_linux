// XMLite.h: interface for the XMLite class.
//
// XMLite : XML Lite Parser Library
// by bro ( Cho,Kyung Min: bro@shinbiro.com ) 2002-10-30
// History.
// 2002-10-29 : First Coded. Parsing XMLElelement and Attributes.
//              get xml parsed string ( looks good )
// 2002-10-30 : Get Node Functions, error handling ( not completed )
// 2002-12-06 : Helper Funtion string to long
// 2002-12-12 : Entity Helper Support
// 2003-04-08 : Close, 
// 2003-07-23 : add property escape_value. (now no escape on default)
// 2003-10-24 : bugfix) attribute parsing <tag a='1' \r\n/> is now ok
// 2004-03-05 : add branch copy functions
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_)
#define AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Windows includes
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>

// Standard Library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Standard Template Library includes
#include <vector>

#include "../AxPortLib/ttchar.h"
#include "../AxPortLib/ttstring.h"

struct _tagXMLAttr;
typedef _tagXMLAttr XAttr, *LPXAttr;
typedef std::vector<LPXAttr> XAttrs;
struct _tagXMLNode;
typedef _tagXMLNode XNode, *LPXNode;
typedef std::vector<LPXNode> XNodes, *LPXNodes;

// Entity Encode/Decode Support
typedef struct _tagXmlEntity {
    TCHAR entity;                   // entity ( & " ' < > )
    TCHAR ref[10];                  // entity reference ( &amp; &quot; etc )
    int ref_len;                    // entity reference length
} XENTITY, *LPXENTITY;

typedef struct _tagXMLEntitys : public std::vector<XENTITY> {
    LPXENTITY GetEntity(int entity);
    LPXENTITY GetEntity(LPTSTR entity);   
    size_t GetEntityCount(LPCTSTR str);
    size_t Ref2Entity(LPCTSTR estr, LPTSTR str, size_t strlen);
    size_t Entity2Ref(LPCTSTR str, LPTSTR estr, size_t estrlen);
    axpl::ttstring Ref2Entity(LPCTSTR estr);
    axpl::ttstring Entity2Ref(LPCTSTR str); 

    _tagXMLEntitys(){};
    _tagXMLEntitys(LPXENTITY entities, size_t count);
} XENTITYS,*LPXENTITYS;

extern XENTITYS entityDefault;
axpl::ttstring XRef2Entity(LPCTSTR estr);
axpl::ttstring XEntity2Ref(LPCTSTR str);    

typedef enum {
    PIE_PARSE_WELFORMED = 0,
    PIE_ALONE_NOT_CLOSED,
    PIE_NOT_CLOSED,
    PIE_NOT_NESTED,
    PIE_ATTR_NO_VALUE
} PCODE;

// Parse info.
typedef struct _tagParseInfo {
    bool        trim_value;         // [set] do trim when parse?
    bool        entity_value;       // [set] do convert from reference to entity? ( &lt; -> < )
    LPXENTITYS  entitys;            // [set] entity table for entity decode
    TCHAR       escape_value;       // [set] escape value (default '\\')

    LPTSTR      xml;                // [get] xml source
    bool        erorr_occur;        // [get] is occurance of error?
    LPTSTR      error_pointer;      // [get] error position of xml source
    PCODE       error_code;         // [get] error code
    axpl::ttstring     error_string;       // [get] error string

    _tagParseInfo() { trim_value = true; entity_value = true; entitys = &entityDefault; xml = NULL; erorr_occur = false; error_pointer = NULL; error_code = PIE_PARSE_WELFORMED; escape_value = _T('\\'); }
} PARSEINFO, *LPPARSEINFO;
extern PARSEINFO piDefault;

// display optional environment
typedef struct _tagDispOption
{
    bool newline;           // newline when new tag
    bool reference_value;   // do convert from entity to reference ( < -> &lt; )
    TCHAR value_quotation_mark;  // val="" (default value quotation mark "
    LPXENTITYS  entitys;    // entity table for entity encode

    int tab_base;           // internal usage
    _tagDispOption() { newline = true; reference_value = true; entitys = &entityDefault; tab_base = 0; value_quotation_mark = _T('"'); }
} DISP_OPT, *LPDISP_OPT;

extern DISP_OPT optDefault;

// XAttr : Attribute Implementation
typedef struct _tagXMLAttr {
    axpl::ttstring name;
    axpl::ttstring value;

    _tagXMLNode*    parent;

    axpl::ttstring GetXML( LPDISP_OPT opt = &optDefault );
} XAttr, *LPXAttr;

// XMLNode structure
typedef struct _tagXMLNode {
    // name and value
    axpl::ttstring name;
    axpl::ttstring value;

    // internal variables
    LPXNode parent;     // parent node
    XNodes childs;     // child node
    XAttrs attrs;      // attributes

    // Load/Save XML
    LPTSTR Load(LPCTSTR pszXml, LPPARSEINFO pi = &piDefault);
    LPTSTR LoadAttributes( LPCTSTR pszAttrs, LPPARSEINFO pi = &piDefault);
    axpl::ttstring GetXML(LPDISP_OPT opt = &optDefault);

    // in own attribute list
    LPXAttr GetAttr(LPCTSTR attrname ); 
    LPCTSTR GetAttrValue( LPCTSTR attrname ); 
    XAttrs GetAttrs(LPCTSTR name ); 

    // in one level child nodes
    LPXNode GetChild(LPCTSTR name ); 
    LPCTSTR GetChildValue(LPCTSTR name ); 
    XNodes GetChilds(LPCTSTR name ); 
    XNodes GetChilds(); 

    LPXAttr GetChildAttr(LPCTSTR name, LPCTSTR attrname );
    LPCTSTR GetChildAttrValue(LPCTSTR name, LPCTSTR attrname );

    // modify DOM 
    size_t GetChildCount();
    XNode *GetChild(size_t i);
    XNodes::iterator GetChildIterator(LPXNode node);
    LPXNode CreateNode(LPCTSTR name = NULL, LPCTSTR value = NULL);
    LPXNode AppendChild(LPCTSTR name = NULL, LPCTSTR value = NULL);
    LPXNode AppendChild(LPXNode node);
    bool RemoveChild(LPXNode node);
    LPXNode DetachChild(LPXNode node);

    // node/branch copy
    void CopyNode( LPXNode node );
    void CopyBranch( LPXNode branch );
    void _CopyBranch( LPXNode node );
    LPXNode AppendChildBranch( LPXNode node );

    // modify attribute
    XAttr *GetAttr(size_t i);
    XAttrs::iterator GetAttrIterator(LPXAttr node);
    LPXAttr CreateAttr(LPCTSTR anem = NULL, LPCTSTR value = NULL);
    LPXAttr AppendAttr(LPCTSTR name = NULL, LPCTSTR value = NULL);
    LPXAttr AppendAttr(LPXAttr attr);
    bool RemoveAttr(LPXAttr attr);
    LPXAttr DetachAttr(LPXAttr attr);

    // operator overloads
    LPXNode operator [] (int i) { return GetChild(i); }
    XNode& operator = (XNode& node) { CopyBranch(&node); return *this; }

    _tagXMLNode() { parent = NULL; }
    ~_tagXMLNode();

    void Close();
} XNode, *LPXNode;

// Helper Funtion
inline long XStr2Int(LPCTSTR str, long default_value = 0) {
    return str ? _ttol(str) : default_value;
}

/// \brief Test if string is empty or white-space only
/// \param str The string to test
/// \return true if string is empty or only white space
inline bool XIsEmptyString(LPCTSTR str) {
    axpl::ttstring s(str);
    return s.empty() || s.find_first_not_of(_T(" \t\n\r")) == s.npos;
    //s.TrimLeft();
    //s.TrimRight();

    //return ( s.IsEmpty() || s == _T("") );
}

#endif // !defined(AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_)
