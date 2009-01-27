<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml"/>

<!--
    The <xsl:apply-templates> element first selects a set of nodes using the
  expression specified in the select attribute. If this attribute is left
  unspecified, all children of the current node are selected. For each of the
  selected nodes, <xsl:apply-templates> directs the XSLT processor to find an
  appropriate <xsl:template> to apply. Templates are tested for applicability
  by comparing the node to the XPath expression specified in the template's
  match attribute. 

    If more than one template satisfies the match pattern, the
  one appearing with the highest priority is chosen. If several templates have
  the same priority, the last in the style sheet is chosen.
-->

<xsl:template match="/ | @* | node()">
  <xsl:copy><xsl:apply-templates select="@* | node()"/></xsl:copy>
</xsl:template>

<xsl:template match="file">
  <xsl:if test="type='file'"><xsl:text disable-output-escaping="yes">&lt;file&gt;</xsl:text></xsl:if>
  <xsl:if test="type='dir'"><xsl:text disable-output-escaping="yes">&lt;dir&gt;</xsl:text></xsl:if>
  <xsl:apply-templates/>
  <xsl:if test="type='file'"><xsl:text disable-output-escaping="yes">&lt;/file&gt;</xsl:text></xsl:if>
  <xsl:if test="type='dir'"><xsl:text disable-output-escaping="yes">&lt;/dir&gt;</xsl:text></xsl:if>
</xsl:template>

<xsl:template match="type">
</xsl:template>

</xsl:stylesheet>
