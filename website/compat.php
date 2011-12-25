<?php
/**
 * @author Casey Bird <FromCrimsonToWool+CommanderGenius@Gmail.com>
 */

    include("/home/project-web/clonekeenplus/htdocs/custom/templates/site/header.php");
    include("/home/project-web/clonekeenplus/htdocs/custom/templates/site/contentHeader.php");
?>
        <div id= "sidebar">
            <img src= "/img/lastlvlep1.jpg" height= "157" width= "150" alt= "Spot - The Yorp" />
            <h6>Commander Keen on modern systems!</h6>
            <p>Enjoy the old but good games with better graphics, sound and controls.</p>
            <h6>Commander Keen 4-6!</h6>
            <p>We are working hard to make Commander Genius compatible with the Galaxy games.</p>
            <a href= "http://sourceforge.net/project/project_donations.php?group_id=249154">
                <img alt= "Please support our project" height= "32" width= "88" src= "http://images.sourceforge.net/images/project-support.jpg" />
            </a>
        </div>

        <div id= "mainContent">
            <div></div>
            <h1>Compatibilty Chart</h1>
            <p>This section shows the compatibility of the games, and what problems have been detected so far:</p>

            <table>
                <tr>
                    <th>Game</th>
                    <th>Compatibility/Completness</th>
                    <th>Known issues</th>
                </tr>
                <tr>
                    <td>Commander Keen: Marooned on Mars</td>
                    <td>95 %</td>
                    <td><ul><li>minor issues</li></ul></td>
                </tr>
                <tr>
                    <td>Commander Keen: Invasion of the Vorticons</td>
                    <td>85 %</td>
                    <td><ul><li>minor issues</li></ul></td>
                </tr>
                <tr>
                    <td>Commander Keen: Keen Must Die</td>
                    <td>85 %</td>
                    <td><ul><li>minor issues</li></ul></td>
                </tr>
                <tr>
                    <td>Keen Dreams</td>
                    <td>0 %</td>
                    <td></td>
                </tr>
                <tr>
                    <td>Commander Keen: Secret of the Oracle</td>
                    <td>10 %</td>
                    <td><ul><li>
                        Keen can walk on the world map.
                        Menu works, but that's all
                    </li></ul></td>
                </tr>
                <tr>
                    <td>Commander Keen Episode 5</td>
                    <td>0 %</td>
                    <td></td>
                </tr>
                <tr>
                    <td>Commander Keen Episode 6</td>
                    <td>0 %</td>
                    <td></td>
                </tr>
            </table>

            <h1>Mods</h1>
            <p>
                Yeah, that's another situation and question. Some mods already work,
                and also some special ones as CG can read Patch code. It does not work
                everything in some of them, but many are supported. Just test it and
                let us know, if something has to be fixed to get the mod working
                correctly.
            </p>

            <p>
                Tulip (Martin Hauber) created a special page where he tested those. Check it out!
            </p>

            <a href= "http://www.shikadi.net/keenwiki/Commander_Genius">
                <strong>Commander Genius Wiki Page</strong>
            </a>
        </div>

<?php
    include("/home/project-web/clonekeenplus/htdocs/custom/templates/site/contentFooter.php");
    include("/home/project-web/clonekeenplus/htdocs/custom/templates/site/footer.php");
?>