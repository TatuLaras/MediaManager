# MediaManager
MediaManager is a program that scans your media folder, fetches all the necessary metadata and images from TMDB and displays it on a clean user interface. It's similar to Kodi, however I did not like using it for a couple of reasons, so I made my own version. 

![image](https://github.com/TatuLaras/MediaManager/assets/34749827/3e49473d-18ac-4ffa-9698-3cbd810982de)


## Reasons for making this and advantages over Kodi
### Way less bloated
MediaManager is laser focused on it's main job, which is cataloguing your media collection and keeping track of what TV show episodes and movies you have already watched. The binaries are somewhere around 10MB uncompressed, so it's definitely smaller than Kodi. It's also way more lightweight, using an UI made with Dear ImGui.

### Snappier UI
The UI has been designed to work well as a desktop program, as well as being keyboard navigatable from the couch. I found Kodi's UI to be slow and unresponsive by default, due to the emphasis of it being a TV app with fancy animations and stuff.

### All video files are visible
MediaManager shows all your media files no matter what. You can manually hide them but all your media files will be visible by default.

![image](https://github.com/TatuLaras/MediaManager/assets/34749827/1a08e9a6-0168-49c2-8f14-3b72422ee0bb)

### Custom metadata search by TMDB ID
In case MediaManager cannot parse the correct TV show or movie from your filenames, you can manually bind a TMDB ID and get all the correct metadata.

![image](https://github.com/TatuLaras/MediaManager/assets/34749827/4f78cd35-4c53-4643-8833-f50c4a0b7d85)

### Bring your own media player
MediaManager doesn't include an integrated media player and is designed to use the media player that you already use and love, be it MPV, VLC etc. The reason for making it this way is that I already have a media player that I like, and even though Kodi provides an integrated media player, I always used my own instead.

![image](https://github.com/TatuLaras/MediaManager/assets/34749827/2d1afa8d-82a6-4641-b344-c3cc07e7adc6)


## Instructions

When you first start using MediaManager, you should go to Menu > Options (or press F1) and configure a few things.

### Media folders
![image](https://github.com/TatuLaras/MediaManager/assets/34749827/17d1b166-84aa-4d7c-bbbc-1a8cec8e0ca9)

Tell MediaManager where you keep your movies and where you keep your TV shows. The folder structure of the movie folder doesn't matter. In regards to the TV show folder, all top level folders under that folder should be individual TV shows named after the TV show's name, under which the episodes can be in a directory structure of your choosing.

### TMDB API
![image](https://github.com/TatuLaras/MediaManager/assets/34749827/229494bf-44da-483f-ba3a-ce3a69a08a27)

You will need a TMDB API bearer key to be able to fetch metadata. You can generate one for free at [https://www.themoviedb.org/settings/api](https://www.themoviedb.org/settings/api).

### Media player
![image](https://github.com/TatuLaras/MediaManager/assets/34749827/8ad88963-1af5-45c4-925a-fe3a342139bc)

Provide a CLI command to run your media player. The command will be run as {configured command} "{the media file}".

---

I mainly made this project for my own media consumption needs, but if someone gets use out of this, that would be awesome. You can build the source in visual studio by opening the .sln file, and installing curl and glfw3 from vcpkg if you haven't already. I have also uploaded some binaries in the releases section on GitHub.

![image](https://github.com/TatuLaras/MediaManager/assets/34749827/5defbc78-343f-48cc-8197-71777f24b859)

