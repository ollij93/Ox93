#pragma once

#ifndef OX93_CHUNKSTREAM_H__
#define OX93_CHUNKSTREAM_H__

// TinyXml...
#include "TinyXML/tinyxml2.h"

class Ox93_ChunkStream
{
public:
	Ox93_ChunkStream(tinyxml2::XMLElement* pxElement);
	Ox93_ChunkStream(tinyxml2::XMLElement* pxElement, tinyxml2::XMLDocument* pxDoc);
	~Ox93_ChunkStream();
	
	void operator >>(float& f) const;
	void operator <<(float f);

	void operator >>(bool& b) const;
	void operator <<(bool b);


private:
	mutable tinyxml2::XMLElement* m_pxElement;
	tinyxml2::XMLElement* m_pxParentElement;
	tinyxml2::XMLDocument* m_pxDoc;
};

#endif // OX93_CHUNKSTREAM_H__

