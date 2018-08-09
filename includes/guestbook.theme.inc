<?php

/**
 * @file
 * Includes preprocess functions for theme of guestbook.
 */

/**
 * Preprocess function for main html file.
 *
 * These variables will be passed to main html file.
 *
 * @param $vars
 */
function guestbook_preprocess_theme(&$vars) {
  $vars['messages'] = Messages::guest_book_get_messages(); // Get messages from database.
  $vars['is_logged'] = session::guest_book_user_is_logged(); // True if user is logged.
  $vars['name'] = isset($_SESSION['user']['name']) ? $_SESSION['user']['name'] : NULL; // Get user name.
}
