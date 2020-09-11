# Acknowledgements
This compiler could not have been created without the guidance of Drew Davidson, an excellent professor who created
the C subset that this compiler operates over. 

## On the naming of our C subset
Our class took a vote and chose HolyC. We are not associated with Terry Davis, his estate, or TempleOS in any way. This
language is not equivalent to "actual" HolyC and will not work as a compiler for TempleOS. Neither the name of this 
compiler nor the name of the C subset over which it operates is to be taken as an endorsement of Terry Davis or his
views.

# Errata
For whatever reason,
```
./holycc <infile> -p
```
is not the correct syntax for testing the parser. Please instead use
```
./holycc -p <infile>
```
at this time.

# Contributors
- Zachary Pearson
- Jacob Swearingen
