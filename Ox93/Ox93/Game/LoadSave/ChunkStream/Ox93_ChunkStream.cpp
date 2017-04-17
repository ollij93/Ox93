#include "Ox93_ChunkStream.h"

// Constructor for load-streams
Ox93_ChunkStream::Ox93_ChunkStream(tinyxml2::XMLElement* pxElement)
: m_pxElement(pxElement)
, m_pxParentElement(nullptr)
, m_pxDoc(nullptr)
{
}

// Constructor for save-streams
Ox93_ChunkStream::Ox93_ChunkStream(tinyxml2::XMLElement* pxElement, tinyxml2::XMLDocument* pxDoc)
: m_pxElement(nullptr)
, m_pxParentElement(pxElement)
, m_pxDoc(pxDoc)
{
	Ox93_Assert(m_pxParentElement, "ERROR: NULL element pointer passed to chunkstream for construction!");
}

Ox93_ChunkStream::~Ox93_ChunkStream()
{
}

void Ox93_ChunkStream::operator>>(float& f) const
{ 
	Ox93_Assert(!m_pxDoc, "Attempting to read from a stream set up for saving");
	if (m_pxElement && !m_pxDoc)
	{
		f = m_pxElement->FloatAttribute("value");

		m_pxElement = m_pxElement->NextSiblingElement("Member");
	}
}

void Ox93_ChunkStream::operator<<(float f)
{
	Ox93_Assert(m_pxDoc, "Attempting to write to a stream not set up for saving");
	if (m_pxParentElement && m_pxDoc)
	{
		m_pxElement = m_pxDoc->NewElement("Member");
		m_pxElement->SetAttribute("value", f);
		m_pxParentElement->LinkEndChild(m_pxElement);
	}
}

void Ox93_ChunkStream::operator>>(bool& b) const
{
	Ox93_Assert(!m_pxDoc, "Attempting to read from a stream set up for saving");
	if (m_pxElement && !m_pxDoc)
	{
		b = m_pxElement->BoolAttribute("value");

		m_pxElement = m_pxElement->NextSiblingElement("Member");
	}
}

void Ox93_ChunkStream::operator<<(bool b)
{
	Ox93_Assert(m_pxDoc, "Attempting to write to a stream not set up for saving");
	if (m_pxParentElement && m_pxDoc)
	{
		m_pxElement = m_pxDoc->NewElement("Member");
		m_pxElement->SetAttribute("value", b);
		m_pxParentElement->LinkEndChild(m_pxElement);
	}
}
