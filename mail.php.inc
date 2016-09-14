<?php

class afmail {

	static function send($headers, $subject, $body, $attach=false) {
		global $site, $afurl, $af;

		require_once('_phpmailer/class.phpmailer.php');
		require_once('_phpmailer/class.smtp.php');

		$mail = new PHPMailer;

		if ($af->debug()) $mail->SMTPDebug = 2;

		if (!empty($attach)) {
			if (is_file($attach)) {
				$mail->addAttachment($attach);
			} else {
				$path = '/tmp/' . md5(microtime(true) . rand()) . '.pdf';
				$x = @copy($attach, $path);
				if ($x) $mail->addAttachment($path);
			}
		}

		if (!isset($headers['from']))		$headers['from']		= $site['af_email'];
		if (!isset($headers['return']))		$headers['return']		= $headers['from'];
		if (!isset($headers['reply']))		$headers['reply']		= $headers['from'];
		if (!isset($headers['priority']))	$headers['priority']	= 3;


		$mail->msgHTML($body);

		if (isset($headers['to'])  &&  tbx_array($headers['to'])) {
			$mail->AddAddress( reset($headers['to']), end($headers['to']) );
		} else if (!empty($headers['to'])) {
			$mail->AddAddress($headers['to']);
		}

		if (isset($headers['cc'])  &&  tbx_array($headers['cc'])) {
			$mail->AddCC( reset($headers['cc']), end($headers['cc']) );
		} else if (!empty($headers['cc'])) {
			$mail->AddCC($headers['cc']);
		}

		if (isset($headers['bcc'])  &&  tbx_array($headers['bcc'])) {
			$mail->AddBCC( reset($headers['bcc']), end($headers['bcc']) );
		} else if (!empty($headers['bcc'])) {
			$mail->AddBCC($headers['bcc']);
		}

		if (isset($headers['reply'])  &&  tbx_array($headers['reply'])) {
			$mail->AddReplyTo( reset($headers['reply']), end($headers['reply']) );
		} else if (!empty($headers['reply'])) {
			$mail->AddReplyTo($headers['reply']);
		}

		if (isset($headers['from'])  &&  tbx_array($headers['from'])) {
			$mail->SetFrom( reset($headers['from']), end($headers['from']) );
		} else if (!empty($headers['from'])) {
			$mail->SetFrom($headers['from']);
		}

		if (!empty($headers['return'])) {
			$mail->Sender = tbx_array($headers['return']) ? reset($headers['return']) : $headers['return'];
		}

		if (!empty($site['af_smtp'])) {
			$mail->IsSMTP();
			$mail->Host	= $site['af_smtp'];

			$mail->SMTPOptions = ['ssl' => [
				'verify_peer'		=> false,
				'verify_peer_name'	=> false,
				'allow_self_signed'	=> true,
			]];
		}

		$mail->Priority	= $headers['priority'];
		$mail->Hostname	= $afurl->domain;
		$mail->Subject	= $subject;
		$mail->XMailer	= altaform::$version;
		$mail->WordWrap	= 80;

		$return = $mail->send();

		if (!empty($path)) @unlink($path);

		if (!$return  &&  $af->debug()) {
			error500($mail->ErrorInfo);
		}

		return $return;
	}




	static function encode($address) {
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




	static function validdns($domain) {
		if (checkdnsrr($domain . '.', 'MX')) return true;
		if (checkdnsrr($domain . '.', 'A' )) return true;
		return false;
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

	static function validate($email, &$matches=false) {
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


}
