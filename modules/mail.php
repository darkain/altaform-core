<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// SEND AN EMAIL USING PHP MAILER INSTEAD OF PHP'S BUILT IN MAIL FUNCTION
////////////////////////////////////////////////////////////////////////////////
function mail($headers, $subject, $body, $attach=false) {
	global $afurl, $af;

	require_once(is_owner('_phpmailer/src/Exception.php'));
	require_once(is_owner('_phpmailer/src/PHPMailer.php'));
	require_once(is_owner('_phpmailer/src/SMTP.php'));

	//Really gaiz, really? double-name space OF THE SAME NAME
	//and THEN the class name, which is also the same!?
	$mail = new \PHPMailer\PHPMailer\PHPMailer;

	if ($af->debug()) $mail->SMTPDebug = 2;

	if (!empty($attach)) {
		if (is_file($attach)) {
			$mail->addAttachment($attach);
		} else {
			$path = sys_get_temp_dir() . '/' . hash_hmac('sha256', $body, microtime(true).rand()) . '.pdf';
			$x = @copy($attach, $path);
			if ($x) $mail->addAttachment($path);
		}
	}

	if (!isset($headers['from']))		$headers['from']		= $af->config->email;
	if (!isset($headers['return']))		$headers['return']		= $headers['from'];
	if (!isset($headers['reply']))		$headers['reply']		= $headers['from'];
	if (!isset($headers['priority']))	$headers['priority']	= 3;


	//THIS FIXES URLS STARTING WITHOUT PROTOCOL, THEN ADDS BODY TO EMAIL
	$mail->msgHTML(str_replace(
		['"//',							"'//"],
		['"'.$afurl->protocol.'://',	"'".$afurl->protocol.'://'],
		$body
	));

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

	if (!empty($af->config->smtp)) {
		$mail->IsSMTP();
		$mail->Host			= $af->config->smtp;

		$mail->SMTPOptions	= ['ssl' => [
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
		throw new Exception($mail->ErrorInfo);
	}

	return $return;
}




////////////////////////////////////////////////////////////////////////////////
// RENDER AN EMAIL AND SEND IT
////////////////////////////////////////////////////////////////////////////////
namespace af\mail;
function render($headers, $subject, $template, $merge=false, $attach=false) {
	global $af;

	$body  = $af->headerEmail();
	$body .= $af->load($template)->merge($merge)->renderToString();
	$body .= $af->footerEmail();

	return \af\mail($headers, $subject, $template, $attach);
}




////////////////////////////////////////////////////////////////////////////////
// ENCODE AN EMAIL ADDRESS - THIS ISN'T SECURE, JUST OBFUSCATING
////////////////////////////////////////////////////////////////////////////////
namespace af\mail;
function encode($address) {
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




////////////////////////////////////////////////////////////////////////////////
// VALIDATE THE DNS OF AN EMAIL ADDRESS. THIS DOES *NOT* MEAN THE INBOX EXISTS
////////////////////////////////////////////////////////////////////////////////
namespace af\mail;
function dns($domain) {
	static $cache = [];

	if (array_key_exists($domain, $cache)) {
		return $cache[$domain];

	} else if (checkdnsrr($domain . '.', 'MX')) {
		return ($cache[$domain] = true);

	} else if (checkdnsrr($domain . '.', 'A' )) {
		return ($cache[$domain] = true);
	}

	return ($cache[$domain] = false);
}




////////////////////////////////////////////////////////////////////////////////
// VALIDATE THAT A GIVEN STRING IS A PROPER EMAIL ADDRESS
////////////////////////////////////////////////////////////////////////////////
namespace af\mail;
function validate($email) {
	if (empty($email)  ||  !is_string($email)) return false;
	return filter_var($email, FILTER_VALIDATE_EMAIL) !== false;
}
