<?php

/**
 * @file
 * Contains php function for start main.php page.
 */

define('GUESTBOOK_ROOT', getcwd());

// Enable display all errors.
error_reporting(E_ALL);
ini_set('display_errors', '1');

require_once GUESTBOOK_ROOT . '/includes/guestbook.core.inc';
require_once GUESTBOOK_ROOT . '/includes/guestbook.sql.inc';
require_once GUESTBOOK_ROOT . '/includes/guestbook.theme.inc';

//guestbook_session_start();
//guestbook_menu_handler();

// Start session.
session::guest_book_session_start();

// Routing.
$my_uri = new uri();
$my_router = new router();
$my_router->route($my_uri->get_uri());


require_once GUESTBOOK_ROOT . '/templates/html.php';
