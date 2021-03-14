#include <cafel/utils/xmlReader.h>

CAFEL_NAMESPACE_BEGIN

SceneNode* XmlReader::analyse(TiXmlElement* pRootEle)
{ 
	TiXmlElement* pEle = pRootEle;
	SceneNode *node = new SceneNode(pEle->Value());

	for (TiXmlAttribute* p_attr = pEle->FirstAttribute(); p_attr; p_attr = p_attr->Next())
	{
		node->addParam(p_attr->Name(), p_attr->Value());
	}

	for (TiXmlElement* pChild = pEle->FirstChildElement(); pChild; pChild = pChild->NextSiblingElement())
	{
		node->addChild(pChild->Value(),analyse(pChild));
	}

	return node;
}

CAFEL_NAMESPACE_END