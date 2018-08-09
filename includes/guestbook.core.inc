<?php

/**
 * @file
 * Includes core functions for guestbook.
 */

/**
 * Returns the equivalent of Apache's $_SERVER['REQUEST_URI'] variable.
 *
 * Because $_SERVER['REQUEST_URI'] is only available on Apache, we generate an
 * equivalent using other environment variables.
 */
class uri {
  public $_SERVER;
  public $uri;
  public function get_uri() {
    if (isset($_SERVER['REQUEST_URI'])) {
      $this->uri = $_SERVER['REQUEST_URI'];
    }
    else {
      $this->uri = $_SERVER['SCRIPT_NAME'];
      if (isset($_SERVER['argv'])) {
        $this->uri = $_SERVER['SCRIPT_NAME'] . '?' . $_SERVER['argv'][0];
      }
      elseif (isset($_SERVER['QUERY_STRING'])) {
        $this->uri = $_SERVER['SCRIPT_NAME'] . '?' . $_SERVER['QUERY_STRING'];
      }
    }
    return $this->uri;
  }
}

class router {
  public $route;
  public function route($uri) {
    // Pars url.
    $route = urldecode(parse_url($uri, PHP_URL_PATH));
    $query = urldecode(parse_url($uri, PHP_URL_QUERY));
    $route = rtrim($route, "/");
    parse_str($query, $outputQuery);

    // Create an array '$outputRoute' that contains the path, id, page, and other data.
    if (isset($outputQuery['page'])) {
      $outputRoute['page'] = (int) $outputQuery['page'];
    }
    $outputRoute['route'] = $route;
//print $outputRoute['route'];
    $route = preg_split("[/]", $route);
    if (count($route) >= 4) {
      if ($route[1] == 'message' &&  $route[3] == 'save') {
        $outputRoute['route'] = '/message/edit';
        $outputRoute['id'] = (int) $route[2];
      }
      elseif ($route[1] == 'message' && $route[3] == 'delete') {
        $outputRoute['route'] = '/message/delete';
        $outputRoute['id'] = (int) $route[2];
      }
    }

    // Mapping.
    $mapping = [
      "/login"           => ['controller' => "login", 'action' => 'guest_book_login'],
      "/logout"          => ['controller' => "login", 'action' => 'guest_book_logout'],
      "/message/delete"  => ['controller' => "message", 'action' => 'guest_book_message_delete'],
      "/message/edit"    => ['controller' => "message", 'action' => 'guest_book_message_save'],
      "/message/save"    => ['controller' => "message", 'action' => 'guest_book_message_save'],
    ];

    // Routing.
    if (isset($mapping[$outputRoute['route']])) {
      $controller = $mapping[$outputRoute['route']]['controller'];
      $controller_obj = new $controller();
      $controller_obj->$mapping[$outputRoute['route']]['action']($outputRoute);
    }
  }

}

class login {
  public function guest_book_login() {
    // Verify that user is exist and verified successfully.
    $user_data = guestBookGetUserData::guest_book_get_user_data($_POST['login'], $_POST['password']);
    if (empty($user_data)) {
      redirect::guest_book_goto();
    }
    else {
      cookie::guest_book_set_cookie();
      // Do not store password in session.
      unset($user_data['password']);
      $user = $user_data + array(
          'time'=> time(),
          'sid' => session_id(),
          'session_cookie' => rand() + rand(),
        );
      $_SESSION['user'] = $user;
      redirect::guest_book_goto();
    }

  }

  public function guest_book_logout() {
    cookie::guest_book_unset_cookie();
    redirect::guest_book_goto();
  }
}

class message {
  public function guest_book_message_delete($data) {
    Messages::guestbook_delete_message($data);
  }

  public function guest_book_message_save($data) {
    Messages::guestbook_save_message($data);
  }
}

class redirect {
  public static function guest_book_goto($path = '', $http_response_code = 302) {
    $path = empty($path) ? '/' : $path;
    header('Location: ' . $path, TRUE, $http_response_code);
    exit();
  }
}

class session {
  public static function guest_book_session_start() {
    if (!session_id()) {
      // Save current session data before starting it, as PHP will destroy it.
      $session_data = isset($_SESSION) ? $_SESSION : NULL;
      session_start();
      // Restore session data.
      if (!empty($session_data)) {
        $_SESSION = $session_data;
      }
    }
  }
  public static function guest_book_user_is_logged() {
    return isset($_COOKIE['guestbook_cookie_id']) ? TRUE : FALSE;
  }
}

class cookie {
  /**
   * Set a cookie.
   */
  public static function guest_book_set_cookie() {
    setcookie('guestbook_cookie_id', session_id());
  }

  /**
   * Unset a cookie.
   */
  public static function guest_book_unset_cookie() {
    unset ($_SESSION['user']);
    unset ($_SESSION['time']);
    setcookie('guestbook_cookie_id');
  }
}

class pagination {
  public static function guest_book_pagination($lim = 10, $prev = 2, $total_rows = 1){
    $pages = array();
    // Count the total number of pages.
    $total_pages = (int) ceil($total_rows / $lim);
    if (getPages::guestbook_get_page()) {
      $total_pages = (int) ceil(getPages::guestbook_get_page() / $lim);
    }
    // Set the current page number.
    if (!isset($_GET['page']) || ($_GET['page'] > $total_pages) || ($_GET['page'] < 1)) {
      $_GET['page'] = 1;
    }
    $page = (int) $_GET['page'];
    // Calculating the range of pages.
    if ($page + $prev >= $total_pages) {
      $page_last = $total_pages;
    }
    else if ($page + $prev <= ($prev *2) + 1) {
      $page_last = ($prev *2) + 1;
    }
    else {
      $page_last = $page + $prev;
    }
    if ($page - $prev <= 1) {
      $page_first = 1;
    }
    else {
      $page_first = $page_last - ($prev *2);
    }
    $range = $page_last - $page_first;
    // Creating an array of pages.
    for ($i = 0; $i <= $range; $i++) {
      $pages[$i] = (int) $page_first + $i;
    }
    $pages['page'] = $page;
    $pages['first'] = 1;
    if ($page - 1 >= 1) {
      $pages['previous'] = $page - 1;
    }
    else {
      $pages['previous'] = 'do not show if string';
    }
    if ($page < $total_pages) {
      $pages['next'] = $page + 1;
    }
    else {
      $pages['next'] = 'do not show if string';
    }
    $pages['last'] = $total_pages;
    // Output page array.
    return $pages;
  }
}