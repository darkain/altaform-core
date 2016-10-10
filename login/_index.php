<?php

$af->renderPage(
	$user->loggedIn() ? 'logout.tpl' : 'login.tpl'
);
