<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- axfc ver. 0.5.1 -->

<!-- :ver: -->

<xsl:output method="html"/>

<xsl:template match="/">

  <html>
  <head>
  <title><xsl:value-of select="/catalog/@name"/></title>
  <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1"/>
  <link type="text/css" href="axfc.css" rel="stylesheet"/>
  
  </head>
    
    <body>

    <div align="center">
    <div class="content" align="center">

    <xsl:for-each select="/catalog/disk">

    <div class="disk">
      <div class="name"><xsl:value-of select="name"/></div>
      <xsl:if test="count(category) &gt; 0">
	<div class="category">Category: <xsl:value-of select="category"/></div>
      </xsl:if>
      <xsl:if test="count(description) &gt; 0">
	<div class="description">Description: <xsl:value-of select="description"/></div>
      </xsl:if>
      <xsl:if test="count(label) &gt; 0">
	  <xsl:for-each select="label">
	      <div class="label">- <xsl:value-of select="."/></div>
	  </xsl:for-each>
      </xsl:if>

      <div class="fileList1">
	<ul>
	<xsl:for-each select="file">
	  <xsl:call-template name="fileOrDirDescription"/>
	  <div class="fileList2">
	    <ul>
	    <xsl:for-each select="file">
	      <xsl:call-template name="fileOrDirDescription"/>
	      <div class="fileList3">
		<ul>
		  <xsl:for-each select="file">
		    <xsl:call-template name="fileOrDirDescription"/>
		    <div class="fileList4">
		      <ul>
			<xsl:for-each select="file">
			  <xsl:call-template name="fileOrDirDescription"/>
			  <ul>
			    <xsl:apply-templates select="file">
			      <xsl:with-param name="path" select="name"/>
			    </xsl:apply-templates>
			  </ul>
			</xsl:for-each>
		      </ul>
		    </div>
		  </xsl:for-each>
		</ul>
	      </div>
	    </xsl:for-each>
	    </ul>
	  </div>
	</xsl:for-each>
	</ul>
    </div>

    </div>
    </xsl:for-each>

    </div>
    </div>

  </body>
  </html>

</xsl:template>

<xsl:template match="file">
  <xsl:param name="path"/>
  <xsl:if test="type='file'">
    <li class="file">
      <xsl:value-of select="$path"/>/<xsl:value-of select="name"/> -
      <span class="details"> - 
      size: <xsl:value-of select="size"/> B
      </span>
      <xsl:if test="count(description) &gt; 0">
	- <span class="description"><xsl:value-of select="description"/></span>
      </xsl:if>
      <xsl:if test="count(label) &gt; 0">
	(
	<xsl:for-each select="label">
	  <span class="label"><xsl:value-of select="."/></span>
	  <xsl:if test="position() != last()">
	    ,
	  </xsl:if>
	</xsl:for-each>
	)
      </xsl:if>      
    </li>
  </xsl:if>
  <xsl:if test="type='dir'">
    <li class="dir">
      <xsl:value-of select="$path"/>/<xsl:value-of select="name"/>
      <xsl:if test="count(description) &gt; 0">
	- <span class="description"><xsl:value-of select="description"/></span>
      </xsl:if>
      <xsl:if test="count(label) &gt; 0">
	(
	<xsl:for-each select="label">
	  <span class="label"><xsl:value-of select="."/></span>
	  <xsl:if test="position() != last()">
	    ,
	  </xsl:if>
	</xsl:for-each>
	)
      </xsl:if>      

    </li>
  </xsl:if>	  
  
  <xsl:apply-templates select="file">
    <xsl:with-param name="path" select="concat($path, '/', name)"/>
  </xsl:apply-templates>
</xsl:template>


<xsl:template name="fileOrDirDescription">
  <xsl:if test="type='file'">
    <li class="file">
      <xsl:value-of select="name"/>
      <span class="details"> - 
      size: <xsl:value-of select="size"/> B
      </span>
      <xsl:if test="count(description) &gt; 0">
	- <span class="description"><xsl:value-of select="description"/></span>
      </xsl:if>
      <xsl:if test="count(label) &gt; 0">
	(
	<xsl:for-each select="label">
	  <span class="label"><xsl:value-of select="."/></span>
	  <xsl:if test="position() != last()">
	    ,
	  </xsl:if>
	</xsl:for-each>
	)
      </xsl:if>      
    </li>
  </xsl:if>
  <xsl:if test="type='dir'">
    <li class="dir">
      <xsl:value-of select="name"/>
      <xsl:if test="count(description) &gt; 0">
	- <span class="description"><xsl:value-of select="description"/></span>
      </xsl:if>
      <xsl:if test="count(label) &gt; 0">
	(
	<xsl:for-each select="label">
	  <span class="label"><xsl:value-of select="."/></span>
	  <xsl:if test="position() != last()">
	    ,
	  </xsl:if>
	</xsl:for-each>
	)
      </xsl:if>      
    </li>
  </xsl:if>	  
</xsl:template>

</xsl:stylesheet>
