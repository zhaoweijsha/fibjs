/*
 * XmlDocument.cpp
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlDocument.h"
#include "XmlElement.h"
#include "XmlAttr.h"
#include "XmlText.h"
#include "XmlComment.h"
#include "XmlCDATASection.h"
#include "XmlProcessingInstruction.h"
#include "XmlParser.h"
#include "parse.h"
#include "encoding_iconv.h"

namespace fibjs
{

DECLARE_MODULE(xml);

result_t XmlDocument_base::_new(const char *type, obj_ptr<XmlDocument_base> &retVal,
                                v8::Local<v8::Object> This)
{
    bool isXml = !qstrcmp(type, "text/xml");

    if (!isXml && qstrcmp(type, "text/html"))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new XmlDocument(isXml);

    return 0;
}

result_t xml_base::parse(const char *source, const char *type, obj_ptr<XmlDocument_base> &retVal)
{
    bool isXml = !qstrcmp(type, "text/xml");

    if (!isXml && qstrcmp(type, "text/html"))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new XmlDocument(isXml);

    return retVal->load(source);
}

result_t xml_base::parse(Buffer_base *source, const char *type, obj_ptr<XmlDocument_base> &retVal)
{
    bool isXml = !qstrcmp(type, "text/xml");

    if (!isXml && qstrcmp(type, "text/html"))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new XmlDocument(isXml);

    return retVal->load(source);
}

result_t xml_base::serialize(XmlNode_base *node, std::string &retVal)
{
    return node->toString(retVal);
}

result_t XmlDocument::get_nodeName(std::string &retVal)
{
    retVal = "#document";
    return 0;
}

result_t XmlDocument::get_nodeValue(std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::set_nodeValue(const char *newVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlDocument::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlDocument::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlDocument::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlDocument::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlDocument::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlDocument::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlDocument::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlDocument::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlDocument::checkNode(XmlNode_base *newChild)
{
    int32_t type;
    newChild->get_nodeType(type);

    if (type == xml_base::_ELEMENT_NODE)
    {
        if (m_element)
        {
            if (m_element != newChild)
                return Runtime::setError("XmlDocument: The document node contains only one element node.");
        }
        else
            m_element = (XmlElement_base *)newChild;
    }
    else if (type == xml_base::_DOCUMENT_TYPE_NODE)
    {
        if (m_doctype)
        {
            if (m_doctype != newChild)
                return Runtime::setError("XmlDocument: The document node contains only one element node.");
        }
        else
            m_doctype = (XmlDocumentType_base *)newChild;
    }

    return 0;
}

result_t XmlDocument::lookupPrefix(const char *namespaceURI, std::string &retVal)
{
    if (globalPrefix(namespaceURI, retVal))
        return 0;

    if (!m_element)
        return CALL_RETURN_NULL;

    return m_element->lookupPrefix(namespaceURI, retVal);
}

result_t XmlDocument::lookupNamespaceURI(const char *prefix, std::string &retVal)
{
    if (globalNamespaceURI(prefix, retVal))
        return 0;

    if (!m_element)
        return CALL_RETURN_NULL;

    return m_element->lookupNamespaceURI(prefix, retVal);
}

result_t XmlDocument::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                                   obj_ptr<XmlNode_base> &retVal)
{
    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlDocument::insertAfter(XmlNode_base *newChild, XmlNode_base *refChild,
                                  obj_ptr<XmlNode_base> &retVal)
{
    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlDocument::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                                   obj_ptr<XmlNode_base> &retVal)
{
    if (newChild == oldChild)
    {
        retVal = oldChild;
        return 0;
    }

    if (oldChild == m_element)
        m_element.Release();
    else if (oldChild == m_doctype)
        m_doctype.Release();

    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlDocument::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    if (oldChild == m_element)
        m_element.Release();
    else if (oldChild == m_doctype)
        m_doctype.Release();

    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlDocument::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    result_t hr = checkNode(newChild);
    if (hr < 0)
        return hr;

    return m_childs->appendChild(newChild, retVal);
}

result_t XmlDocument::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlDocument::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    obj_ptr<XmlDocument> doc = new XmlDocument(*this);
    return XmlNodeImpl::cloneNode(doc, deep, retVal);
}

result_t XmlDocument::normalize()
{
    return m_childs->normalize();
}

result_t XmlDocument::load(const char *source)
{
    return m_isXml ? XmlParser::parse(this, source) : XmlParser::parseHtml(this, source);
}

result_t XmlDocument::load(Buffer_base *source)
{
    std::string strBuf;
    result_t hr;

    source->toString(strBuf);

    if (!m_isXml)
    {
        _parser p(strBuf);
        const char *ptr;

        while ((ptr = qstristr(p.now(), "<meta")) != NULL && qisspace(ptr[5]))
        {
            bool bContentType = false;
            std::string content;

            p.pos = (int32_t)(ptr - p.string + 5);
            while (true)
            {
                std::string key, value;

                p.skipSpace();
                p.getWord(key, '=', '>');

                if (key.empty())
                    break;

                if (p.want('='))
                {
                    if (p.want('\"'))
                    {
                        p.getString(value, '\"', '>');
                        p.want('\"');
                    }
                    else
                        p.getWord(value, '>');
                }

                if (!qstricmp(key.c_str(), "charset"))
                {
                    m_encoding = value;
                    break;
                }
                else if (!qstricmp(key.c_str(), "content"))
                    content = value;
                else if (!qstricmp(key.c_str(), "http-equiv") &&
                         !qstricmp(value.c_str(), "Content-Type"))
                    bContentType = true;
            }

            if (bContentType && !content.empty())
            {
                _parser p1(content);

                while (true)
                {
                    std::string key, value;

                    p1.skipSpace();
                    p1.getWord(key, ';', '=');

                    if (key.empty())
                        break;

                    if (p1.want('='))
                        p1.getWord(value, ';');

                    p1.want(';');

                    if (!qstricmp(key.c_str(), "charset"))
                    {
                        m_encoding = value;
                        break;
                    }
                }
            }

            if (!m_encoding.empty())
                break;
        }

        if (!m_encoding.empty())
        {
            encoding_iconv conv(m_encoding.c_str());
            conv.decode(strBuf, strBuf);
        }
    }

    hr = load(strBuf.c_str());
    if (hr < 0)
        return hr;

    return 0;
}

result_t XmlDocument::get_doctype(obj_ptr<XmlDocumentType_base> &retVal)
{
    if (!m_doctype)
        return CALL_RETURN_NULL;

    retVal = m_doctype;
    return 0;
}

result_t XmlDocument::get_documentElement(obj_ptr<XmlElement_base> &retVal)
{
    if (!m_element)
        return CALL_RETURN_NULL;

    retVal = m_element;
    return 0;
}

result_t XmlDocument::createElement(const char *tagName, obj_ptr<XmlElement_base> &retVal)
{
    retVal = new XmlElement(this, tagName, m_isXml);
    return 0;
}

result_t XmlDocument::createElementNS(const char *namespaceURI, const char *qualifiedName,
                                      obj_ptr<XmlElement_base> &retVal)
{
    retVal = new XmlElement(this, namespaceURI, qualifiedName, m_isXml);
    return 0;
}

result_t XmlDocument::createTextNode(const char *data, obj_ptr<XmlText_base> &retVal)
{
    retVal = new XmlText(this, data);
    return 0;
}

result_t XmlDocument::createComment(const char *data, obj_ptr<XmlComment_base> &retVal)
{
    retVal = new XmlComment(this, data);
    return 0;
}

result_t XmlDocument::createCDATASection(const char *data, obj_ptr<XmlCDATASection_base> &retVal)
{
    retVal = new XmlCDATASection(this, data);
    return 0;
}

result_t XmlDocument::createProcessingInstruction(const char *target, const char *data,
        obj_ptr<XmlProcessingInstruction_base> &retVal)
{
    retVal = new XmlProcessingInstruction(this, target, data);
    return 0;
}

result_t XmlDocument::get_head(obj_ptr<XmlElement_base> &retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    XmlElement *pEl = (XmlElement *)(XmlElement_base *)m_element;
    if (pEl)
        return pEl->getFirstElementsByTagName("head", retVal);

    return CALL_RETURN_NULL;
}

result_t XmlDocument::get_title(std::string &retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    XmlElement *pEl = (XmlElement *)(XmlElement_base *)m_element;
    if (pEl)
    {
        obj_ptr<XmlElement_base> title;
        if (pEl->getFirstElementsByTagName("title", title) == CALL_RETURN_NULL)
            return 0;

        return title->get_textContent(retVal);
    }

    return 0;
}

result_t XmlDocument::get_body(obj_ptr<XmlElement_base> &retVal)
{
    if (m_isXml)
        return CALL_E_INVALID_CALL;

    XmlElement *pEl = (XmlElement *)(XmlElement_base *)m_element;
    if (pEl)
        return pEl->getFirstElementsByTagName("body", retVal);

    return CALL_RETURN_NULL;
}

result_t XmlDocument::getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList_base> &retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    XmlElement *pEl = (XmlElement *)(XmlElement_base *)m_element;

    if (pEl)
        pEl->getElementsByTagNameFromThis(tagName, ret);

    retVal = ret;
    return 0;
}

result_t XmlDocument::getElementsByTagNameNS(const char *namespaceURI, const char *localName,
        obj_ptr<XmlNodeList_base> &retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    XmlElement *pEl = (XmlElement *)(XmlElement_base *)m_element;

    if (pEl)
        pEl->getElementsByTagNameNSFromThis(namespaceURI, localName, ret);

    retVal = ret;
    return 0;
}

result_t XmlDocument::get_inputEncoding(std::string &retVal)
{
    if (m_encoding.empty())
        return CALL_RETURN_NULL;

    retVal = m_encoding;
    return 0;
}

result_t XmlDocument::get_xmlStandalone(bool &retVal)
{
    if (m_standalone < 0)
        return CALL_RETURN_NULL;

    retVal = m_standalone == 1;
    return 0;
}

result_t XmlDocument::set_xmlStandalone(bool newVal)
{
    m_standalone = newVal ? 1 : 0;
    return 0;
}

result_t XmlDocument::get_xmlVersion(std::string &retVal)
{
    if (m_version.empty())
        retVal = "1.0";
    else
        retVal = m_version;
    return 0;
}

result_t XmlDocument::set_xmlVersion(const char *newVal)
{
    m_version = newVal;
    return 0;
}

result_t XmlDocument::toString(std::string &retVal)
{
    std::string strChilds;
    m_childs->toString(strChilds);

    if (!m_version.empty())
    {
        retVal = "<?xml version=\"";
        retVal.append(m_version);

        if (!m_encoding.empty())
        {
            retVal.append("\" encoding=\"");
            retVal.append(m_encoding);
        }

        if (m_standalone >= 0)
            retVal.append(m_standalone ? "\" standalone=\"yes\"?>" : "\" standalone=\"no\"?>");
        else
            retVal.append("\"?>");

        retVal.append(strChilds);
    }
    else
        retVal = strChilds;

    return 0;
}

}
