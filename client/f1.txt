#include"minishell.h"

extern process *chld;

/*name : fore_ground_process 
 *input : fg command
 *output : nothing
 *description : put the process into fore ground if any of them has stopped */


int  fore_ground_prcess(char ** arg)
{
    int status;

#if 0
    /*checking the input*/
    for(int i = 0; arg[i] != NULL; i++)
    printf("fb:%s\n",arg[i]);
#endif

    
    
	process * temp = chld;

	/* if the only fg is given*/
	if(arg[1] == NULL)
	{
	
	while(temp->next != NULL) 
	    temp = temp->next;
	} 

	/*if pid is mentioned*/
	else
	{
	    while(temp->next != NULL)
	    {
		if(temp->pid == atoi(arg[1]))
		    break;
		temp = temp->next;
	    }
	}

	printf("%s\n", temp->proc_name);
	/*send the SIGCONT to the respective process*/
	kill(temp->pid , SIGCONT);
	/*wiat for child to terminate*/
	waitpid(temp->pid , &status , 0);
	/*delete the info */
	delete_job(temp->pid);

	return status;
}


int  back_ground_process(char ** arg)
{
#if 0
    /*checking the input*/
    for(int i = 0; arg[i] != NULL; i++)
    printf("fb:%s\n",arg[i]);
#endif
	process * temp = chld;

	/*sigaction parameters*/
	struct sigaction act ;
	memset(&act , 0 , sizeof(act));
	act.sa_sigaction = signal_chld;
	act.sa_flags = SA_SIGINFO;

	/* if the only bg is given*/
	if(arg[1] == NULL)
	{
	
	while(temp->next != NULL) 
	    temp = temp->next;
	} 

	/*if pid is mentioned*/
	else
	{
	    while(temp->next != NULL)
	    {
		if(temp->pid == atoi(arg[1]))
		    break;
		temp = temp->next;
	    }
	}


	printf("%s:>\n",getcwd(NULL , 0));
	printf("[%d] %s\n" , temp->indx , temp->proc_name);
	kill(temp->pid , SIGCONT);

	sigaction(SIGCHLD , &act , NULL);
//	waitpid(temp->pid , NULL , WCONTINUED);
	delete_job(temp->pid);
	return 0;
}
	







