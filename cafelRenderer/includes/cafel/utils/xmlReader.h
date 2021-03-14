#pragma once

#if !defined(_CAFEL_XML_READER_HPP_)
#define _CAFEL_XML_READER_HPP_

#include <cafel/system/sceneNode.h>
#include <tinnyxml/tinystr.h>
#include <tinnyxml/tinyxml.h>

#include <string>

CAFEL_NAMESPACE_BEGIN

class XmlReader
{
public:
	static SceneNode* readXmlFile(std::string file)
	{
		TiXmlDocument *pDoc = new TiXmlDocument();
		pDoc->LoadFile(file.c_str());
		TiXmlElement *RootElem = pDoc->RootElement();//root elem
		return analyse(RootElem);
	}

private:
	XmlReader();

	static SceneNode* analyse(TiXmlElement* pRootEle);
};

CAFEL_NAMESPACE_END

#endif 