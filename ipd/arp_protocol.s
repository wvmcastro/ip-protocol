	.file	"arp_protocol.c"
	.text
	.globl	buildArpRequest
	.type	buildArpRequest, @function
buildArpRequest:
.LFB31:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	%edx, -24(%rbp)
	movl	$0, -6(%rbp)
	movw	$0, -2(%rbp)
	leaq	-6(%rbp), %rcx
	movl	-24(%rbp), %edx
	movq	-32(%rbp), %rsi
	movl	-20(%rbp), %eax
	movl	$1, %r8d
	movl	%eax, %edi
	call	buildArpPacket
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE31:
	.size	buildArpRequest, .-buildArpRequest
	.globl	buildArpReply
	.type	buildArpReply, @function
buildArpReply:
.LFB32:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -8(%rbp)
	movq	%rcx, -24(%rbp)
	movq	-24(%rbp), %rcx
	movl	-8(%rbp), %edx
	movq	-16(%rbp), %rsi
	movl	-4(%rbp), %eax
	movl	$2, %r8d
	movl	%eax, %edi
	call	buildArpPacket
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE32:
	.size	buildArpReply, .-buildArpReply
	.globl	buildArpPacket
	.type	buildArpPacket, @function
buildArpPacket:
.LFB33:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	movl	%edx, -56(%rbp)
	movq	%rcx, -72(%rbp)
	movl	%r8d, %eax
	movw	%ax, -76(%rbp)
	movl	$28, -4(%rbp)
	movl	$14, -8(%rbp)
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	cltq
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -16(%rbp)
	movb	$-1, -38(%rbp)
	movb	$-1, -37(%rbp)
	movb	$-1, -36(%rbp)
	movb	$-1, -35(%rbp)
	movb	$-1, -34(%rbp)
	movb	$-1, -33(%rbp)
	cmpw	$1, -76(%rbp)
	je	.L6
	movq	-72(%rbp), %rax
	movl	(%rax), %edx
	movl	%edx, -38(%rbp)
	movzwl	4(%rax), %eax
	movw	%ax, -34(%rbp)
.L6:
	movq	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-64(%rbp), %rdx
	leaq	-38(%rbp), %rsi
	movq	-24(%rbp), %rax
	movl	$2054, %ecx
	movq	%rax, %rdi
	call	fillEthernetHeader@PLT
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -32(%rbp)
	movl	$1, %edi
	call	htons@PLT
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movw	%dx, (%rax)
	movl	$2048, %edi
	call	htons@PLT
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movw	%dx, 2(%rax)
	movq	-32(%rbp), %rax
	movb	$6, 4(%rax)
	movq	-32(%rbp), %rax
	movb	$4, 5(%rax)
	movzwl	-76(%rbp), %eax
	movl	%eax, %edi
	call	htons@PLT
	movl	%eax, %edx
	movq	-32(%rbp), %rax
	movw	%dx, 6(%rax)
	movq	-32(%rbp), %rax
	leaq	8(%rax), %rcx
	movq	-64(%rbp), %rax
	movl	$6, %edx
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	memcpy@PLT
	movl	-52(%rbp), %eax
	movl	%eax, %edi
	call	htonl@PLT
	movl	%eax, -44(%rbp)
	movq	-32(%rbp), %rax
	leaq	14(%rax), %rdx
	movl	-44(%rbp), %eax
	movl	%eax, (%rdx)
	movq	-32(%rbp), %rax
	leaq	18(%rax), %rcx
	movq	-72(%rbp), %rax
	movl	$6, %edx
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	memcpy@PLT
	movl	-56(%rbp), %eax
	movl	%eax, %edi
	call	htonl@PLT
	movl	%eax, -44(%rbp)
	movq	-32(%rbp), %rax
	leaq	24(%rax), %rdx
	movl	-44(%rbp), %eax
	movl	%eax, (%rdx)
	movq	-16(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE33:
	.size	buildArpPacket, .-buildArpPacket
	.globl	sendArpPacket
	.type	sendArpPacket, @function
sendArpPacket:
.LFB34:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	leaq	-32(%rbp), %rax
	movl	$20, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-48(%rbp), %rax
	addq	$16, %rax
	movq	%rax, %rdi
	call	if_nametoindex@PLT
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	testl	%eax, %eax
	jne	.L9
	movl	$1, %edi
	call	exit@PLT
.L9:
	movw	$17, -32(%rbp)
	movq	-48(%rbp), %rax
	leaq	38(%rax), %rcx
	leaq	-32(%rbp), %rax
	addq	$12, %rax
	movl	$6, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	movb	$6, -21(%rbp)
	movl	$3, %edi
	call	htons@PLT
	movzwl	%ax, %eax
	movl	%eax, %edx
	movl	$3, %esi
	movl	$17, %edi
	call	_socket@PLT
	movl	%eax, -4(%rbp)
	movl	$42, -8(%rbp)
	movl	-8(%rbp), %edx
	leaq	-32(%rbp), %rcx
	movq	-40(%rbp), %rsi
	movl	-4(%rbp), %eax
	movl	$20, %r9d
	movq	%rcx, %r8
	movl	$0, %ecx
	movl	%eax, %edi
	call	sendto@PLT
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	cmpl	$0, -12(%rbp)
	jg	.L10
	movl	$1, %edi
	call	exit@PLT
.L10:
	movq	-48(%rbp), %rax
	movl	60(%rax), %eax
	leal	1(%rax), %edx
	movq	-48(%rbp), %rax
	movl	%edx, 60(%rax)
	movq	-48(%rbp), %rax
	movq	72(%rax), %rdx
	movl	-12(%rbp), %eax
	cltq
	addq	%rax, %rdx
	movq	-48(%rbp), %rax
	movq	%rdx, 72(%rax)
	movl	-12(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	.L11
	movl	$-1, %eax
	jmp	.L13
.L11:
	movl	$0, %eax
.L13:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE34:
	.size	sendArpPacket, .-sendArpPacket
	.ident	"GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",@progbits
