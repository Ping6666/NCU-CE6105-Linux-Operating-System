// code v1.9

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

unsigned long user_virt_to_phys(struct mm_struct *mm, unsigned long v_addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    unsigned long p_addr = 0;
    unsigned long page_addr = 0;
    unsigned long page_offset = 0;

    /*
    BUG
    */
    // v_addr need to check whether equal to 0

    pgd = pgd_offset(mm, v_addr);
    if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
    {
        printk(KERN_WARNING "calling pgd_offset fail at v_addr: %lx.\n", v_addr);
        goto out;
    }

    p4d = p4d_offset(pgd, v_addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
    {
        printk(KERN_WARNING "calling p4d_offset fail at v_addr: %lx.\n", v_addr);
        goto out;
    }

    pud = pud_offset(p4d, v_addr);
    if (pud_none(*pud) || unlikely(pud_bad(*pud)))
    {
        printk(KERN_WARNING "calling pud_offset fail at v_addr: %lx.\n", v_addr);
        goto out;
    }

    pmd = pmd_offset(pud, v_addr);
    if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd)))
    {
        printk(KERN_WARNING "calling pmd_offset fail at v_addr: %lx.\n", v_addr);
        goto out;
    }

    pte = pte_offset_kernel(pmd, v_addr);
    if (pte_none(*pte))
    {
        printk(KERN_WARNING "calling pte_offset_kernel fail at v_addr: %lx.\n", v_addr);
        goto out;
    }

    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = v_addr & ~PAGE_MASK;
    p_addr = page_addr | page_offset;

    printk(KERN_WARNING "page_addr: %lx, page_offset: %lx\n", page_addr, page_offset);
    printk(KERN_WARNING "v_addr: %lx, p_addr: %lx\n", v_addr, p_addr);

out:
    return p_addr;
}

asmlinkage int sys_get_phy_addr(void *__user user_init_addr, int user_init_addr_len, void *__user user_result_addr, int user_result_addr_len)
{
    /*
    input argv
    */
    // void *__user user_init_addr: copy from user addr
    // int user_init_addr_len: copy from user addr length
    // void *__user user_result_addr: copy to user addr
    // int user_result_addr_len: copy to user addr predict length

    /*
    output argv (int)
    */
    // int: the actual copy to user addr length
    // > 0 with correct exe, <= 0 may come with wrong

    unsigned long *init_addr[user_init_addr_len];
    unsigned long *result_addr[user_result_addr_len];

    int init_addr_index;       // for outer for loop index
    int result_addr_index = 0; // for inner while loop index
    unsigned long copyCheck;

    unsigned long target_v_addr;

    // copy_from_user
    printk(KERN_WARNING "sizeof user_init_addr:\t%ld\n", (sizeof(unsigned long *) * user_init_addr_len));
    copyCheck = copy_from_user(&init_addr, user_init_addr, (sizeof(unsigned long *) * user_init_addr_len));
    if (copyCheck != 0)
    {
        printk(KERN_WARNING "calling copy_from_user with %ld B remaining.\n", copyCheck);
        return -2;
    }

    // page_table_management
    for (init_addr_index = 0; init_addr_index < user_init_addr_len; init_addr_index += 2)
    {
        target_v_addr = (unsigned long)init_addr[init_addr_index];            // start v_addr
        while (target_v_addr < (unsigned long)init_addr[init_addr_index + 1]) // end v_addr
        {
            if (result_addr_index >= user_result_addr_len)
            {
                printk(KERN_WARNING "bad prediction on result_addr_len.\n");
                return -3;
            }
            result_addr[result_addr_index] = (unsigned long *)user_virt_to_phys(current->mm, target_v_addr);

            target_v_addr += PAGE_SIZE;
            result_addr_index += 1;
        }
    }
    printk(KERN_WARNING "calling page_table_management successful.\n");

    // copy_to_user
    printk(KERN_WARNING "sizeof user_result_addr:\t%ld\n", (sizeof(unsigned long *) * user_result_addr_len));
    copyCheck = copy_to_user(user_result_addr, &result_addr, (sizeof(unsigned long *) * user_result_addr_len));
    if (copyCheck != 0)
    {
        printk(KERN_WARNING "calling copy_to_user with %ld B remaining.\n", copyCheck);
        return -4;
    }

    return result_addr_index;
}
