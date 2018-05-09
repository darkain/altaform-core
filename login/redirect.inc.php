<?php

//REDIRECT TO NEW PAGE
$redirect	= $get->sessionClear('redirect');
$referer	= $get->sessionClear('referer');

if (!empty($redirect))
	$afurl->redirect($redirect, 302);

if (!empty($referer)  &&  $referer !== $afurl(['login'], $afurl))
	$afurl->redirect($referer, 302);

//TODO: VERIFY THAT REDIRECT IS WITHIN OUR ACCEPTED DOMAINS LIST (ORIGINS)
if (!empty($get('redirect')))
	$afurl->redirect($get('redirect'), 302);

if (empty($af->config->session['redirect']))
	$afurl->redirect([]);

if ($af->config->session['redirect'] === 'root')
	$afurl->redirect([]);

if ($af->config->session['redirect'] === 'profile') {
	if (!empty($user->user_url)) {
		$afurl->redirect([$user->user_url], 302);
	} else {
		$afurl->redirect([$user->user_id], 302);
	}
}

$afurl->redirect($af->config->session['redirect']);
