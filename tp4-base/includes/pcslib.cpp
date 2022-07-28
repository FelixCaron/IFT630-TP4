/*******************************************************************/
/*                                                                 */
/*  Implantation des fonctions de la classe processus              */
/*                                                                 */
/*******************************************************************/
/*******************************************************************/

#include "pcslib.h"

/*******************************************************************/
/*                                                                 */
/*                  Constructeur                                   */
/*                                                                 */
/*  Cette routine cree un objet processus bidon.  Elle doit etre   */
/*  utilisee conjointement avec la fonction fork.                  */
/*                                                                 */
/*                                                                 */
/*******************************************************************/
Pcs::Pcs() {}

/*******************************************************************/
/*                                                                 */
/*                  Constructeur                                   */
/*                                                                 */
/*  Cette routine permet de creer un processus.  Le processus      */
/*  executera le code contenu dans le fichier specifie en          */
/*  parametre.  La fonction retournera l'identificateur            */
/*  du nouveau processus.                                          */
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*    fichier : ce fichier contient le code executable du nouveau  */
/*              processus.                                         */
/*                                                                 */
/*                                                                 */
/*******************************************************************/
Pcs::Pcs(char *fichier)
{
   if ((pcsid = fork()) == -1)
      perror("Classe Pcs Function Fork (Fork)");
   else
     if (pcsid == 0)
       if (execl(fichier,0)== -1)
          perror("Classe Pcs Function Fork (exec)");
}

/*******************************************************************/
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*    fichier : ce fichier contient le code executable du nouveau  */
/*              processus.                                         */
/*    arg1 : argument to pass to the executable                    */
/*    arg2 : argument to pass to the executable                    */
/*                                                                 */
/*                                                                 */
/*******************************************************************/
Pcs::Pcs(char *fichier, char *const args[])
{
   if ((pcsid = fork()) == -1)
      perror("Classe Pcs Function Fork (Fork)");
   else
     if (pcsid == 0)
       if (execv(fichier, &args[0])== -1)
          perror("Classe Pcs Function Fork (exec)");
}
/*******************************************************************/
/*                                                                 */
/*                  F O R K                                        */
/*                                                                 */
/*  Cette routine permet de creer un processus.  Le processus      */
/*  executera le code contenu dans le fichier specifie en          */
/*  parametre.  La fonction retournera l'identificateur (pcsid)    */
/*  du nouveau processus.                                          */
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*    fichier : ce fichier contient le code executable du nouveau  */
/*              processus.                                         */
/*                                                                 */
/*******************************************************************/


void Pcs::Fork(char *fichier)
{
   if ((pcsid = fork()) == -1)
      perror("Classe Pcs Function Fork (Fork)");
   else
     if (pcsid == 0)
       if (execl(fichier,0)== -1)
          perror("Classe Pcs Function Fork (exec)");
}

/*******************************************************************/
/*                                                                 */
/*                D e t r u i t                                  */
/*                                                                 */
/*  Cette routine detruit le processus                             */
/*                                                                 */
/*******************************************************************/
void Pcs::Detruit()
{
  if (kill(pcsid, SIGKILL) == -1)
     perror("Classe Pcs Function Kill");
}

/*******************************************************************/
/*                                                                 */
/*                        J o i n                                  */
/*                                                                 */
/*  Cette routine permet au processus appelant d'attendre la fin   */
/*  de l'execution d'un processus qu'il a cree avant de poursuivre */
/*  la sienne.                                                     */
/*                                                                 */
/*******************************************************************/

int Pcs::Join()
{
   if ((pcsid = waitpid(pcsid,&status,WUNTRACED)) == -1)
      perror("Classe Pcs Function Join");
   return(status);

}
