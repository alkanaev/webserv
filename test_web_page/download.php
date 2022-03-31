<?php
    if(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on') 
        $url = "https"; 
    else
        $url = "http"; 
      
    // Ajoutez // à l'URL.
    $url .= "://"; 
      
    // Ajoutez l'hôte (nom de domaine, ip) à l'URL.
    if (isset($_SERVER['REMOTE_ADDR'])) {
    $host = $_SERVER['REMOTE_ADDR'];
    }
    $url .= $host . '/';
      
    // Afficher l'URL
    if ($_SERVER['REQUEST_METHOD'] == 'POST') 
    {
        $link = htmlspecialchars($_POST['my_download']);
        $path = $link;
        $path = str_replace($url, "", $path);
        $name = basename($path);
        $path = './html/' . $path;
        if (!file_exists($path))
		{
			echo 'Chemin invalide<br />';
			exit;
		}
        echo("Clic ici pour telecharger: <a href=$link download=''>$name</a>");
    }
    else
        echo "Il y a ue un probleme"
?>