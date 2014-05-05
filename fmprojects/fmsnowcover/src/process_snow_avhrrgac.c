



<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<!-- ViewVC :: http://www.viewvc.org/ -->
<head>
<title>[cryoclim] Log of /snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c</title>
<meta name="generator" content="ViewVC 1.1.5" />
<link rel="shortcut icon" href="/viewvc/*docroot*/images/favicon.ico" type="image/x-icon" />
<link rel="stylesheet" href="/viewvc/*docroot*/styles.css" type="text/css" />

</head>
<body>
<div class="vc_navheader">
<table><tr>
<td><strong><a href="/viewvc"><span class="pathdiv">/</span></a><a href="/viewvc/cryoclim/">[cryoclim]</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/">snow_satproc</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/trunk/">trunk</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/">OSI_HL_Ice</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/">modules</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/">mod_avhrrgac</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/">swathproc</a><span class="pathdiv">/</span><a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/">src</a><span class="pathdiv">/</span>process_snow_avhrrgac.c</strong></td>
<td style="text-align: right;"></td>
</tr></table>
</div>
<div style="float: right; padding: 5px;"><a href="http://www.viewvc.org/" title="ViewVC Home"><img src="/viewvc/*docroot*/images/viewvc-logo.png" alt="ViewVC logotype" width="240" height="70" /></a></div>
<h1>Log of /snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c</h1>

<p style="margin:0;">

<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/"><img src="/viewvc/*docroot*/images/back_small.png" class="vc_icon" alt="Parent Directory" /> Parent Directory</a>

| <a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?view=log"><img src="/viewvc/*docroot*/images/log.png" class="vc_icon" alt="Revision Log" /> Revision Log</a>




</p>

<hr />
<table class="auto">



<tr>
<td>Links to HEAD:</td>
<td>
(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?view=markup">view</a>)
(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?view=co">download</a>)

(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?view=annotate">annotate</a>)
</td>
</tr>



<tr>
<td>Sticky Revision:</td>
<td><form method="get" action="/viewvc/cryoclim" style="display: inline">
<div style="display: inline">
<input type="hidden" name="orig_pathtype" value="FILE"/><input type="hidden" name="orig_view" value="log"/><input type="hidden" name="orig_path" value="snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c"/><input type="hidden" name="view" value="redirect_pathrev"/>

<input type="text" name="pathrev" value="" size="6"/>

<input type="submit" value="Set" />
</div>
</form>

</td>
</tr>
</table>
 







<div>
<hr />

<a name="rev171"></a>


Revision <a href="/viewvc/cryoclim?view=revision&amp;revision=171"><strong>171</strong></a> -


(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?revision=171&amp;view=markup">view</a>)


(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?revision=171&amp;view=co">download</a>)

(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?annotate=171">annotate</a>)



- <a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?r1=171&amp;view=log">[select for diffs]</a>




<br />

Modified

<em>Thu Dec 12 14:01:20 2013 UTC</em>
(3 months, 2 weeks ago)
by <em>mariak</em>









<br />File length: 43292 byte(s)







<br />Diff to <a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?r1=151&amp;r2=171">previous 151</a>







<pre class="vc_log">Metop1 and Metop2 included.
</pre>
</div>



<div>
<hr />

<a name="rev151"></a>


Revision <a href="/viewvc/cryoclim?view=revision&amp;revision=151"><strong>151</strong></a> -


(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?revision=151&amp;view=markup">view</a>)


(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?revision=151&amp;view=co">download</a>)

(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?annotate=151">annotate</a>)



- <a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?r1=151&amp;view=log">[select for diffs]</a>




<br />

Modified

<em>Tue Aug  6 14:05:59 2013 UTC</em>
(7 months, 3 weeks ago)
by <em>mariak</em>









<br />File length: 43094 byte(s)







<br />Diff to <a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?r1=131&amp;r2=151">previous 131</a>







<pre class="vc_log">minor updates for older sensors and improving outfiles
</pre>
</div>



<div>
<hr />

<a name="rev131"></a>


Revision <a href="/viewvc/cryoclim?view=revision&amp;revision=131"><strong>131</strong></a> -


(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?revision=131&amp;view=markup">view</a>)


(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?revision=131&amp;view=co">download</a>)

(<a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?annotate=131">annotate</a>)



- <a href="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c?r1=131&amp;view=log">[select for diffs]</a>




<br />

Added

<em>Wed Jul 10 08:03:20 2013 UTC</em>
(8 months, 2 weeks ago)
by <em>mariak</em>







<br />File length: 39250 byte(s)











<pre class="vc_log">New try at intial import</pre>
</div>

 


 <hr />
<p><a name="diff"></a>
This form allows you to request diffs between any two revisions of this file.
For each of the two "sides" of the diff,

enter a numeric revision.

</p>
<form method="get" action="/viewvc/cryoclim/snow_satproc/trunk/OSI_HL_Ice/modules/mod_avhrrgac/swathproc/src/process_snow_avhrrgac.c" id="diff_select">
<table cellpadding="2" cellspacing="0" class="auto">
<tr>
<td>&nbsp;</td>
<td>
<input type="hidden" name="view" value="diff"/>
Diffs between

<input type="text" size="12" name="r1"
value="171" />

and

<input type="text" size="12" name="r2" value="131" />

</td>
</tr>
<tr>
<td>&nbsp;</td>
<td>
Type of Diff should be a
<select name="diff_format" onchange="submit()">
<option value="h" selected="selected">Colored Diff</option>
<option value="l" >Long Colored Diff</option>
<option value="f" >Full Colored Diff</option>
<option value="u" >Unidiff</option>
<option value="c" >Context Diff</option>
<option value="s" >Side by Side</option>
</select>
<input type="submit" value=" Get Diffs " />
</td>
</tr>
</table>
</form>





<hr />
<table>
<tr>
<td><address><a href="mailto:svn@met.no">svn@met.no</a></address></td>
<td style="text-align: right;"><strong><a href="/viewvc/*docroot*/help_log.html">ViewVC Help</a></strong></td>
</tr>
<tr>
<td>Powered by <a href="http://viewvc.tigris.org/">ViewVC 1.1.5</a></td>
<td style="text-align: right;">&nbsp;</td>
</tr>
</table>
</body>
</html>


