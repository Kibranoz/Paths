# paths3





This is the official project for  the Path Editor, which allows the user to set their paths in a GUI manner, similar to what Windows offers





## Why ?


While it is possible to manage your path from the file, some users are less familiar with code like syntax for path and alias management. Some also may find it quicker to open a gui to add their paths rather than add them through .bashrc





This was made as an attempt to make the family of operating system using tools such as the Linux kernel and the GNU library (Commonly refered to as Linux or LiGNUx) to be more user-friendly





## Features


- Use fedora's .bashrc.d folder, so it doesn't mess with your .bashrc file


- It will create a .bashrc.d folder if you are not on a fedora or similar distro and add the following code to your .bashrc file


```


if [ -d ~/.bashrc.d ]; then


    for rc in ~/.bashrc.d/*; do


        if [ -f "$rc" ]; then


            . "$rc"


        fi


    done


fi


unset rc


```


- You can also add aliases, which are user defined scripts





## Limitations


- It does not add your existing paths or aliases.





## Roadmap


- Publish this to flathub store


- Add translations


- more if I want to





## Contributions 





Contributions are welcome, such as





- Potential fixes : I am new to C and gtk programming, if I made things in a way that weren't best practises, I probably did not know. If there are bugs that I did not catch, it goes without saying that you can submit a PR to fix this.


- New logo : The current one is fine, but it's also AI generated with some fixes on my end. If you want to make a real logo for this app, feel free. I would still prefer to keep the original idea behind the logo which is a road to represent a path, but other concepts are welcome if I like it.





- Soon : If you want to make this available in your language, I have not yet found a way to set up multilinguality in this app, but I will accept them eventually. 

