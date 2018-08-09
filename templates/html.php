<?php
/**
 * @file
 * Includes main html.
 */
// Get all necessary theme variables.
guestbook_preprocess_theme($vars);
?>

<html>
  <head>
    <meta charset="utf-8" />
    <link rel="stylesheet" type="text/css" href="/css/guestbook.css" />
    <title>Guest book</title>
  </head>

  <body>
    <div class="header"><h1>Guest Book</h1></div>

    <?php //Show login block if user is not logged, otherwise - "logout" and name. ?>
    <?php if ($vars['is_logged']): ?>
      <div class="blocklogin">
        <form name="logout" action="/logout" method="POST">
          <label><?php print $vars['name']; ?></label>
          <input type="submit" name="signout" value="Sign out" />
        </form>
      </div>
    <?php else: ?>
      <div class="blocklogin">
        <form name="login" action="/login" method="POST">
          <div>
            <label>Login</label>
            <input type="text" name="login" placeholder="login"/>
          </div>
          <div>
            <label>Password</label>
            <input type="password" name="password" placeholder="password"/>
          </div>
          <input class="submit" type="submit" name="loginButton" value="Sign in"/>
        </form>
      </div>
    <?php endif; ?>
    <?php //Show messages block if user is not logged, otherwise - messages and textarea. ?>
    <div class="guestbook-messages">
      <?php if ($vars['messages']): ?>
        <?php foreach ($vars['messages'] as $message): ?>
          <div class="btn-class"><?php print guestBookGetUserData::guestbook_get_name($message[3]); ?>
          <?php print date( 'd.m.y H:i:s ', $message[1] ); ?>
          <?php print "<br/>"; ?>
          <?php print htmlspecialchars($message[2]); ?></div>
          <?php if (isset($_COOKIE['guestbook_cookie_id']) && isset ($_SESSION['user'])): ?>
            <?php if ($message[4] == $_SESSION['user']['session_cookie'] && $_COOKIE['guestbook_cookie_id'] == session_id() && $vars['is_logged']): ?>
              <?php print "<a href='/?id={$message[0]}&edit=1&page={$_GET['page']}' class='push-button blue'> Редактировать </a> ";?>
              <?php print "<a href='/message/{$message[0]}/delete/{$_GET['page']}' class='push-button red'> Удалить </a> "; ?>
            <?php endif; ?>
          <?php endif; ?>
          <br />
        <?php endforeach; ?>
      <?php endif; ?>
    </div>
    <?php if ($vars['is_logged']): ?>
      <div class="textarea">
        <form name="Guestbook_message_send" action="/message/<?php if (isset ($_GET['edit'])) print "{$_GET['id']}/"; ?>save/<?php if (isset ($_GET['edit'])) print "{$_GET['page']}/"; ?>" method="POST" >
          <textarea rows="5" cols="150" name="message"><?php if (isset($_GET['edit']) && $_GET['edit'] == 1): ?>
<?php if ($vars['messages']): ?>
<?php foreach ($vars['messages'] as $message): ?>
<?php if ($message[0] == $_GET['id']): ?>
<?php print $message[2]; ?>
<?php endif; ?>
<?php endforeach; ?>
<?php endif; ?><?php endif; ?></textarea><br /><br />
<input type="submit" name="Guestbook_message_save" class='push-button red' value="Save" />
        </form>
        <?php if (isset($_GET['edit']) && $_GET['edit'] == 1): ?>
          <?php print "<a href='/?page={$_GET['page']}' class='push-button red'> Cancel </a> "; ?>
        <?php endif; ?>
      </div>
    <?php endif; ?>


    <?php //Show pages block. ?>
    <div class="pages">
      <ul class="pagination">
        <?php $pages = pagination::guest_book_pagination(); ?>
          <li><?php print "<a href='?page={$pages['first']}'> &laquoпервая</a> "; ?></li>
          <?php if (is_int ($pages['previous'])): ?>
            <li><?php print "<a href='?page={$pages['previous']}'> &laquoпредыдущая</a> "; ?></li>
          <?php endif; ?>
          <?php foreach( $pages as $key => $value): ?>
            <?php if (is_int ($key)): ?>
              <?php if ($value == $pages['page']): ?>
                <li class="active"><?php print "<a href='?page={$value}'>$value</a> "; ?></li>
              <?php else: ?>
                <li><?php print "<a href='?page={$value}'>$value</a> "; ?></li>
              <?php endif; ?>
            <?php endif; ?>
          <?php endforeach; ?>
        <?php if (is_int ($pages['next'])): ?>
            <li><?php print "<a href='?page={$pages['next']}'> следующая&raquo</a> "; ?></li>
        <?php endif; ?>
        <li><?php print "<a href='?page={$pages['last']}'> последняя&raquo</a> "; ?></li>
      </ul>
    </div>
  </body>
</html>
