<?php


function afmail($headers, $subject, $body) {
	global $site, $og;

	if (!isset($headers['to'])) return false;

	if (!isset($headers['content_type'])) $headers['content_type'] = 'text/html; charset="iso-8859-1"; format=flowed';

	if (!isset($headers['from']))     $headers['from']     = $site['email'];
	if (!isset($headers['return']))   $headers['return']   = $headers['from'];
	if (!isset($headers['reply']))    $headers['reply']    = $headers['from'];
	if (!isset($headers['priority'])) $headers['priority'] = 1;

	if (isset($headers[ 'to'])  &&  is_array($headers[ 'to'])) $headers[ 'to'] = implode(', ', $headers[ 'to']);
	if (isset($headers[ 'cc'])  &&  is_array($headers[ 'cc'])) $headers[ 'cc'] = implode(', ', $headers[ 'cc']);
	if (isset($headers['bcc'])  &&  is_array($headers['bcc'])) $headers['bcc'] = implode(', ', $headers['bcc']);

	$hash	 = '-x-x-' . md5(microtime(true)) . '-y-y-';

	$header	 = "From: $headers[from]\r\n";
	$header	.= "Return-Path: $headers[return]\r\n";
	$header	.= "Reply-To: $headers[reply]\r\n";

	if (isset($headers['cc']))  $header .= "Cc: $headers[cc]\r\n";
	if (isset($headers['bcc'])) $header .= "Bcc: $headers[bcc]\r\n";

	$header	.= "X-Priority: $headers[priority]\r\n";
	$header	.= "X-Mailer: Altaform/1.0\r\n";

	$header	.= "MIME-Version: 1.0\r\n";
	$header	.= "Content-Type: multipart/alternative;\r\n";
	$header	.= " boundary=\"$hash\"\r\n";

	//TODO: add support for arbitrary email headers from the $headers variable

	$subject = str_replace("\r\n", ' ', $subject);
	$subject = str_replace("\r",   ' ', $subject);
	$subject = str_replace("\n",   ' ', $subject);

	$body	 = str_replace("\r\n", "\n", $body);
	$body	 = str_replace("\r",   "\n", $body);

	$rawtext = strip_tags($body);
	$rawtext = join("\n", array_map('trim', explode("\n", $rawtext)));
	$rawtext = trim($rawtext);
	$rawtext = str_replace('&copy;', '(C)', $rawtext);
	$rawtext = html_entity_decode($rawtext);

	$data	 = "This is a multi-part message in MIME format.\r\n--$hash\r\n";

	$data	.= "Content-Type: text/plain\r\n";
	$data	.= "Content-Transfer-Encoding: 7bit\r\n";
	$data	.= "\r\n" . wordwrap($rawtext, 70) . "\r\n\r\n";
	$data	.= "\r\n--$hash\r\n";

	$data	.= "Content-Type: $headers[content_type]\r\n";
	$data	.= "Content-Transfer-Encoding: 7bit\r\n";
	$data	.= "\r\n" . wordwrap($body, 70) . "\r\n\r\n";
	$data	.= "\r\n--$hash--\r\n\r\n";

	return mail(
		$headers['to'],
		$subject,
		$data,
		$header,
		"-f$headers[return]"
	);
}




function encode_email_address($address) {
	$str = '';

	$len = strlen($address);
	for ($i=0; $i<$len; $i++) {
		if (rand(0,1)) {
			$str .= '&#x' . dechex(ord(substr($address, $i, 1))) . ';';
		} else {
			$str .= '&#' . ord(substr($address, $i, 1)) . ';';
		}
	}

	return $str;
}




#
# RFC(2)822 Email Parser
#
# By Cal Henderson <cal@iamcal.com>
# This code is licensed under a Creative Commons Attribution-ShareAlike 2.5 License
# http://creativecommons.org/licenses/by-sa/2.5/
#
# $Revision: 1.1 $
#

##################################################################################

function is_valid_email_address($email, &$matches=false) {
	if (empty($email)  ||  !is_string($email)) return false;


	####################################################################################
	#
	# NO-WS-CTL       =       %d1-8 /         ; US-ASCII control characters
	#                         %d11 /          ;  that do not include the
	#                         %d12 /          ;  carriage return, line feed,
	#                         %d14-31 /       ;  and white space characters
	#                         %d127
	# ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z
	# DIGIT          =  %x30-39

	$no_ws_ctl    = "[\\x01-\\x08\\x0b\\x0c\\x0e-\\x1f\\x7f]";
	$alpha        = "[\\x41-\\x5a\\x61-\\x7a]";
	$digit        = "[\\x30-\\x39]";
	$cr        = "\\x0d";
	$lf        = "\\x0a";
	$crlf        = "($cr$lf)";


	####################################################################################
	#
	# obs-char        =       %d0-9 / %d11 /          ; %d0-127 except CR and
	#                         %d12 / %d14-127         ;  LF
	# obs-text        =       *LF *CR *(obs-char *LF *CR)
	# text            =       %d1-9 /         ; Characters excluding CR and LF
	#                         %d11 /
	#                         %d12 /
	#                         %d14-127 /
	#                         obs-text
	# obs-qp          =       "\" (%d0-127)
	# quoted-pair     =       ("\" text) / obs-qp

	$obs_char    = "[\\x00-\\x09\\x0b\\x0c\\x0e-\\x7f]";
	$obs_text    = "($lf*$cr*($obs_char$lf*$cr*)*)";
	$text        = "([\\x01-\\x09\\x0b\\x0c\\x0e-\\x7f]|$obs_text)";
	$obs_qp        = "(\\x5c[\\x00-\\x7f])";
	$quoted_pair    = "(\\x5c$text|$obs_qp)";


	####################################################################################
	#
	# obs-FWS         =       1*WSP *(CRLF 1*WSP)
	# FWS             =       ([*WSP CRLF] 1*WSP) /   ; Folding white space
	#                         obs-FWS
	# ctext           =       NO-WS-CTL /     ; Non white space controls
	#                         %d33-39 /       ; The rest of the US-ASCII
	#                         %d42-91 /       ;  characters not including "(",
	#                         %d93-126        ;  ")", or "\"
	# ccontent        =       ctext / quoted-pair / comment
	# comment         =       "(" *([FWS] ccontent) [FWS] ")"
	# CFWS            =       *([FWS] comment) (([FWS] comment) / FWS)

	#
	# note: we translate ccontent only partially to avoid an infinite loop
	# instead, we'll recursively strip comments before processing the input
	#

	$wsp        = "[\\x20\\x09]";
	$obs_fws    = "($wsp+($crlf$wsp+)*)";
	$fws        = "((($wsp*$crlf)?$wsp+)|$obs_fws)";
	$ctext        = "($no_ws_ctl|[\\x21-\\x27\\x2A-\\x5b\\x5d-\\x7e])";
	$ccontent    = "($ctext|$quoted_pair)";
	$comment    = "(\\x28($fws?$ccontent)*$fws?\\x29)";
	$cfws        = "(($fws?$comment)*($fws?$comment|$fws))";
	$cfws        = "$fws*";


	####################################################################################
	#
	# atext           =       ALPHA / DIGIT / ; Any character except controls,
	#                         "!" / "#" /     ;  SP, and specials.
	#                         "$" / "%" /     ;  Used for atoms
	#                         "&" / "'" /
	#                         "*" / "+" /
	#                         "-" / "/" /
	#                         "=" / "?" /
	#                         "^" / "_" /
	#                         "`" / "{" /
	#                         "|" / "}" /
	#                         "~"
	# atom            =       [CFWS] 1*atext [CFWS]

	$atext        = "($alpha|$digit|[\\x21\\x23-\\x27\\x2a\\x2b\\x2d\\x2f\\x3d\\x3f\\x5e\\x5f\\x60\\x7b-\\x7e])";
	$atom        = "($cfws?$atext+$cfws?)";


	####################################################################################
	#
	# qtext           =       NO-WS-CTL /     ; Non white space controls
	#                         %d33 /          ; The rest of the US-ASCII
	#                         %d35-91 /       ;  characters not including "\"
	#                         %d93-126        ;  or the quote character
	# qcontent        =       qtext / quoted-pair
	# quoted-string   =       [CFWS]
	#                         DQUOTE *([FWS] qcontent) [FWS] DQUOTE
	#                         [CFWS]
	# word            =       atom / quoted-string

	$qtext        = "($no_ws_ctl|[\\x21\\x23-\\x5b\\x5d-\\x7e])";
	$qcontent    = "($qtext|$quoted_pair)";
	$quoted_string    = "($cfws?\\x22($fws?$qcontent)*$fws?\\x22$cfws?)";
	$word        = "($atom|$quoted_string)";


	####################################################################################
	#
	# obs-local-part  =       word *("." word)
	# obs-domain      =       atom *("." atom)

	$obs_local_part    = "($word(\\x2e$word)*)";
	$obs_domain    = "($atom(\\x2e$atom)*)";


	####################################################################################
	#
	# dot-atom-text   =       1*atext *("." 1*atext)
	# dot-atom        =       [CFWS] dot-atom-text [CFWS]

	$dot_atom_text    = "($atext+(\\x2e$atext+)*)";
	$dot_atom    = "($cfws?$dot_atom_text$cfws?)";


	####################################################################################
	#
	# domain-literal  =       [CFWS] "[" *([FWS] dcontent) [FWS] "]" [CFWS]
	# dcontent        =       dtext / quoted-pair
	# dtext           =       NO-WS-CTL /     ; Non white space controls
	# 
	#                         %d33-90 /       ; The rest of the US-ASCII
	#                         %d94-126        ;  characters not including "[",
	#                                         ;  "]", or "\"

	$dtext        = "($no_ws_ctl|[\\x21-\\x5a\\x5e-\\x7e])";
	$dcontent    = "($dtext|$quoted_pair)";
	$domain_literal    = "($cfws?\\x5b($fws?$dcontent)*$fws?\\x5d$cfws?)";


	####################################################################################
	#
	# local-part      =       dot-atom / quoted-string / obs-local-part
	# domain          =       dot-atom / domain-literal / obs-domain
	# addr-spec       =       local-part "@" domain

	$local_part    = "($dot_atom|$quoted_string|$obs_local_part)";
	$domain        = "($dot_atom|$domain_literal|$obs_domain)";
	$addr_spec     = "($local_part\\x40$domain)";


	#
	# we need to strip comments first (repeat until we can't find any more)
	#

	$done = 0;
	while(!$done) {
		$new = preg_replace("!$comment!", '', $email);
		if (strlen($new) == strlen($email)) $done = 1;
		$email = $new;
	}


	#
	# now match what's left
	#


	# 20080616 - modified output result from "1 : 0" to "true : false"

	return preg_match("!^$addr_spec$!", $email, $matches) ? true : false;
}
