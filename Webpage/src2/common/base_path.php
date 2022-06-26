<?php
    $base_path = $_SERVER['REQUEST_URI'];
    if (strpos($base_path, ".php") !== False) {
      $pos = max(strrpos($base_path, "/src"), strrpos($base_path, "/index.php"));
      $base_path = substr($base_path, 0, $pos);
    }
    $base_path = rtrim($base_path, "/");
    $document_base_path = $_SERVER['DOCUMENT_ROOT'].$base_path;
?>