<?php

/**
 * @file
 * Contains sql connection functions.
 */

/**
 * Mysql connection constants.
 */
define('GUESTBOOK_SQL_HOST', '');
define('GUESTBOOK_SQL_USER', 'root');
define('GUESTBOOK_SQL_PASS', '');
define('GUESTBOOK_SQL_DBNAME', 'guestbook');

class connectDB {
  /**
   * Connects to db.
   *
   * @return \mysqli|null
   */
  public static function guest_book_sql_connect() {
    static $link = NULL;
    if (!$link) {
      $link = mysqli_connect(GUESTBOOK_SQL_HOST, GUESTBOOK_SQL_USER, GUESTBOOK_SQL_PASS, GUESTBOOK_SQL_DBNAME);
      if (!$link) {
        die('Connection failed: ' . mysqli_error($link));
      }
      mysqli_select_db($link, GUESTBOOK_SQL_DBNAME);
    }
    return $link;
  }
}

class Messages {
  /**
   * Returns messages.
   *
   * @param string $order
   * @return array
   */
  public static function guest_book_get_messages($order = 'DESC') {
    if (!isset($_GET['page'])) {
      $_GET['page'] = 1;
    }
    $all = getPages::guestbook_get_page();
    $pages = ceil($all / 10);
    if ($_GET['page'] == 1) {
      $gt = 0;
    }
    else {
      if ($_GET['page'] <= $pages) {
        $gt = ($_GET['page'] * 10) - 10;
      }
      else {
        $gt = 0;
      }
    }
    $limit = ' LIMIT ' . $gt . ', 10';
    $query = mysqli_query(connectDB::guest_book_sql_connect(), 'SELECT * FROM `share` ORDER BY id ' . $order . $limit);
    return $query->fetch_all();
  }

  /**
   * Saves messages.
   *
   * This function is used for creation new messages and edit already existed.
   *
   * @param array $data
   */
  public static function guestbook_save_message($data) {
    // Get info from header.
    if (isset($data['id'])) {
      $id = $data['id'];
    }
    if (isset($data['page'])) {
      $page = $data['page'];
    }
    // Check user is logged.
    $user_is_logged = session::guest_book_user_is_logged();
    if ($user_is_logged === TRUE) {
      // Check id.
      if (isset($id)) {
        // Get info by id.
        $title = guestBookGetUserData::guestbook_get_info($id);
        if ($title['id']) {
          $new = FALSE;
        }
        else {
          // Show warning message if 'id' does not exist in database.
          try {
            throw new Exception("Сообщение не существует. Вернуться на главную страницу: " . "<a href='/'> Главная </a> ");
          }catch (Exception $e) {
            echo $e->getMessage();
          }
        }
      }
      else {
        $new = TRUE;
      }
      // Save message if new = true.
      if ($new == TRUE) {
        if (isset($_POST['message']) && $_POST['message'] != NULL) {
          $message = $_POST['message'];
          $uid = $_SESSION['user']['id'];
          $time = Time();
          $session_cookie = $_SESSION['user']['session_cookie'];
          $query = mysqli_prepare(connectDB::guest_book_sql_connect(), 'INSERT INTO `share`(id, date, message, uid, session_cookie) VALUES(\'NULL\', ?, ?, ?, ?)');
          $query->bind_param('issi', $time, $message, $uid, $session_cookie);
          $query->execute();
          // Checking number of page in header.
          if (isset($page)) {
            $path = '/' . $page;
            // Redirect to page.
            redirect::guest_book_goto($path);
          }
          redirect::guest_book_goto();
        }
        redirect::guest_book_goto();
      }
      // Edit message if new = false.
      else {
        // Check message is not empty.
        if (isset($_POST['message'])) {
          $message = $_POST['message'];
          // Get user id.
          $uid = $_SESSION['user']['id'];
          // Prepare and execute message editing.
          $query = mysqli_prepare(connectDB::guest_book_sql_connect(), 'UPDATE `share` SET message = ? , uid = ? WHERE id = ? ');
          $query->bind_param('ssi', $message, $uid, $id);
          $query->execute();
          // Checking number of page in header.
          if (isset($page)) {
            $path = '/' . $page;
            // Redirect to page.
            redirect::guest_book_goto($path);
          }
          else {
            redirect::guest_book_goto();
          }
        }
        redirect::guest_book_goto();
      }
    }
    // Redirect to main page if user is not logged.
    redirect::guest_book_goto();
  }

  /**
   * Deletes messages.
   *
   * @param array $uri
   */
  public static function guestbook_delete_message($data) {
    // Get info from header.
    if (isset($data['id'])) {
      $id = $data['id'];
    }
    if (isset($data['page'])) {
      $page = $data['page'];
    }
    // Check user is logged.
    $user_is_logged = session::guest_book_user_is_logged();
    if ($user_is_logged === TRUE) {
      // Check id.
      if (isset($id)) {
        // Get info by id.
        $title = guestBookGetUserData::guestbook_get_info($id);
        // Show warning message if 'id' does not exist in database.
        try {
          if (!$title['id']) {
            throw new Exception("Сообщение не существует. Вернуться на главную страницу: " . "<a href='/'> Главная </a> ");
          }
        }catch (Exception $e) {
          echo $e->getMessage();
        }
        // Checking cookies, that would not delete someone else's message.
        if ($_SESSION['user']['session_cookie'] == $title['session_cookie']) {
          // Deleting message.
          $query = mysqli_prepare(connectDB::guest_book_sql_connect(), 'DELETE FROM `share` WHERE id= ? ');
          $query->bind_param('i', $id);
          $query->execute();
        }
      }
      // Checking number of page in header.
      if (isset($page)) {
        $path = '/' . $page;
        // Redirect to page.
        redirect::guest_book_goto($path);
      }
      else {
        redirect::guest_book_goto();
      }
    }
    // Redirect to main page if user is not logged.
    redirect::guest_book_goto();
  }
}

class guestBookGetUserData {
  /**
   * Returns user name.
   *
   * @param int $uid
   * @return string.
   */
  public static function guestbook_get_name($uid) {
    $query = mysqli_prepare(connectDB::guest_book_sql_connect(), 'SELECT * FROM `users` WHERE id = ?');
    $query->bind_param('i', $uid);
    $query->execute();
    $result = $query->get_result();
    $result = $result->fetch_assoc();
    return $result['name'];
  }

  /**
   * Returns user info by id.
   *
   * @param int $id
   * @return array.
   */
  public static function guestbook_get_info($id) {
    $query = mysqli_prepare(connectDB::guest_book_sql_connect(), 'SELECT * FROM `share` WHERE id = ?');
    $query->bind_param('i', $id);
    $query->execute();
    $result = $query->get_result();
    $result = $result->fetch_assoc();
    return $result;
  }

  /**
   * Returns user data.
   *
   * @param string $login
   * @param string $passw
   * @return array
   */
  public static function guest_book_get_user_data($login, $passw) {
    $passw = sha1($passw);
    $query = mysqli_prepare(connectDB::guest_book_sql_connect(), 'SELECT * FROM `users` WHERE login = ? AND password = ?');
    $query->bind_param('ss', $login, $passw);
    $query->execute();
    $result = $query->get_result();
    return $result->fetch_assoc();
  }
}

class getPages {
  /**
   * Returns num rows.
   *
   * @return int.
   */
  public static function guestbook_get_page() {
    $query = mysqli_query(connectDB::guest_book_sql_connect(), "SELECT COUNT(*) as count FROM `share`");
    return $query->fetch_row()[0];
  }
}


