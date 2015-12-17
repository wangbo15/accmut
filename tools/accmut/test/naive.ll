; ModuleID = 'naive.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Mutation = type { i32, i32, i32, i32, i32, i32, i32, i64, i64 }
%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }

@MUTATION_ID = global i32 0, align 4
@__accmut__init.path = private unnamed_addr constant [100 x i8] c"/home/nightwish/tmp/accmut/mutations.txt\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 16
@.str = private unnamed_addr constant [2 x i8] c"r\00", align 1
@.str.1 = private unnamed_addr constant [43 x i8] c"FILE ERROR: mutation.txt can not open !!!\0A\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.3 = private unnamed_addr constant [24 x i8] c"%d:%3s:%*[^:]:%*[^:]:%s\00", align 1
@.str.4 = private unnamed_addr constant [17 x i8] c"%d -- %s --  %s\0A\00", align 1
@.str.5 = private unnamed_addr constant [4 x i8] c"AOR\00", align 1
@.str.6 = private unnamed_addr constant [6 x i8] c"%d:%d\00", align 1
@.str.7 = private unnamed_addr constant [4 x i8] c"LOR\00", align 1
@.str.8 = private unnamed_addr constant [4 x i8] c"ROR\00", align 1
@.str.9 = private unnamed_addr constant [9 x i8] c"%d:%d:%d\00", align 1
@.str.10 = private unnamed_addr constant [4 x i8] c"STD\00", align 1
@.str.11 = private unnamed_addr constant [4 x i8] c"LVR\00", align 1
@.str.12 = private unnamed_addr constant [14 x i8] c"%d:%d:%ld:%ld\00", align 1
@ALLMUTS = common global [10001 x %struct.Mutation*] zeroinitializer, align 16
@.str.13 = private unnamed_addr constant [25 x i8] c"######### INIT END ####\0A\00", align 1
@.str.14 = private unnamed_addr constant [37 x i8] c"ERROR : __accmut__cal_i32_arith !!!\0A\00", align 1
@.str.15 = private unnamed_addr constant [37 x i8] c"ERROR : __accmut__cal_i64_arith !!!\0A\00", align 1
@.str.16 = private unnamed_addr constant [16 x i8] c"FORK ERROR !!!\0A\00", align 1
@.str.17 = private unnamed_addr constant [35 x i8] c"ERROR : __accmut_cal_i32_bool !!!\0A\00", align 1
@.str.18 = private unnamed_addr constant [36 x i8] c"ERROR : __accmut__cal_i64_bool !!!\0A\00", align 1
@.str.19 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define void @__accmut__init() #0 {
entry:
  %path = alloca [100 x i8], align 16
  %fp = alloca %struct._IO_FILE*, align 8
  %id = alloca i32, align 4
  %type = alloca [4 x i8], align 1
  %buff = alloca [50 x i8], align 16
  %useless = alloca [20 x i8], align 16
  %tail = alloca [20 x i8], align 16
  %m = alloca %struct.Mutation*, align 8
  %s_op = alloca i32, align 4
  %t_op = alloca i32, align 4
  %s_op26 = alloca i32, align 4
  %t_op27 = alloca i32, align 4
  %op37 = alloca i32, align 4
  %s_pre = alloca i32, align 4
  %t_pre = alloca i32, align 4
  %op48 = alloca i32, align 4
  %f_tp = alloca i32, align 4
  %op58 = alloca i32, align 4
  %op_i = alloca i32, align 4
  %s_c = alloca i64, align 8
  %t_c = alloca i64, align 8
  %0 = bitcast [100 x i8]* %path to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %0, i8* getelementptr inbounds ([100 x i8], [100 x i8]* @__accmut__init.path, i32 0, i32 0), i64 100, i32 16, i1 false)
  %arraydecay = getelementptr inbounds [100 x i8], [100 x i8]* %path, i32 0, i32 0
  %call = call %struct._IO_FILE* @fopen(i8* %arraydecay, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i32 0, i32 0))
  store %struct._IO_FILE* %call, %struct._IO_FILE** %fp, align 8
  %1 = load %struct._IO_FILE*, %struct._IO_FILE** %fp, align 8
  %cmp = icmp eq %struct._IO_FILE* %1, null
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %call1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([43 x i8], [43 x i8]* @.str.1, i32 0, i32 0))
  call void @exit(i32 0) #6
  unreachable

if.end:                                           ; preds = %entry
  br label %while.cond

while.cond:                                       ; preds = %if.end.66, %if.end
  %arraydecay2 = getelementptr inbounds [50 x i8], [50 x i8]* %buff, i32 0, i32 0
  %2 = load %struct._IO_FILE*, %struct._IO_FILE** %fp, align 8
  %call3 = call i8* @fgets(i8* %arraydecay2, i32 50, %struct._IO_FILE* %2)
  %tobool = icmp ne i8* %call3, null
  br i1 %tobool, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %arraydecay4 = getelementptr inbounds [50 x i8], [50 x i8]* %buff, i32 0, i32 0
  %call5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i32 0, i32 0), i8* %arraydecay4)
  %arraydecay6 = getelementptr inbounds [50 x i8], [50 x i8]* %buff, i32 0, i32 0
  %arraydecay7 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %arraydecay8 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call9 = call i32 (i8*, i8*, ...) @__isoc99_sscanf(i8* %arraydecay6, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.3, i32 0, i32 0), i32* %id, i8* %arraydecay7, i8* %arraydecay8) #7
  %3 = load i32, i32* %id, align 4
  %arraydecay10 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %arraydecay11 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.4, i32 0, i32 0), i32 %3, i8* %arraydecay10, i8* %arraydecay11)
  %call13 = call noalias i8* @malloc(i64 48) #7
  %4 = bitcast i8* %call13 to %struct.Mutation*
  store %struct.Mutation* %4, %struct.Mutation** %m, align 8
  %arraydecay14 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %call15 = call i32 @strcmp(i8* %arraydecay14, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.5, i32 0, i32 0)) #8
  %tobool16 = icmp ne i32 %call15, 0
  br i1 %tobool16, label %if.else, label %if.then.17

if.then.17:                                       ; preds = %while.body
  %5 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %type18 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %5, i32 0, i32 0
  store i32 0, i32* %type18, align 8
  %arraydecay19 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call20 = call i32 (i8*, i8*, ...) @__isoc99_sscanf(i8* %arraydecay19, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.6, i32 0, i32 0), i32* %s_op, i32* %t_op) #7
  %6 = load i32, i32* %s_op, align 4
  %7 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %op = getelementptr inbounds %struct.Mutation, %struct.Mutation* %7, i32 0, i32 1
  store i32 %6, i32* %op, align 4
  %8 = load i32, i32* %t_op, align 4
  %9 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %t_op21 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %9, i32 0, i32 2
  store i32 %8, i32* %t_op21, align 8
  br label %if.end.66

if.else:                                          ; preds = %while.body
  %arraydecay22 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %call23 = call i32 @strcmp(i8* %arraydecay22, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.7, i32 0, i32 0)) #8
  %tobool24 = icmp ne i32 %call23, 0
  br i1 %tobool24, label %if.else.32, label %if.then.25

if.then.25:                                       ; preds = %if.else
  %arraydecay28 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call29 = call i32 (i8*, i8*, ...) @__isoc99_sscanf(i8* %arraydecay28, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.6, i32 0, i32 0), i32* %s_op26, i32* %t_op27) #7
  %10 = load i32, i32* %s_op26, align 4
  %11 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %op30 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %11, i32 0, i32 1
  store i32 %10, i32* %op30, align 4
  %12 = load i32, i32* %t_op27, align 4
  %13 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %t_op31 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %13, i32 0, i32 2
  store i32 %12, i32* %t_op31, align 8
  br label %if.end.65

if.else.32:                                       ; preds = %if.else
  %arraydecay33 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %call34 = call i32 @strcmp(i8* %arraydecay33, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.8, i32 0, i32 0)) #8
  %tobool35 = icmp ne i32 %call34, 0
  br i1 %tobool35, label %if.else.43, label %if.then.36

if.then.36:                                       ; preds = %if.else.32
  %arraydecay38 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call39 = call i32 (i8*, i8*, ...) @__isoc99_sscanf(i8* %arraydecay38, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.9, i32 0, i32 0), i32* %op37, i32* %s_pre, i32* %t_pre) #7
  %14 = load i32, i32* %op37, align 4
  %15 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %op40 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %15, i32 0, i32 1
  store i32 %14, i32* %op40, align 4
  %16 = load i32, i32* %s_pre, align 4
  %17 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %s_pre41 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %17, i32 0, i32 3
  store i32 %16, i32* %s_pre41, align 4
  %18 = load i32, i32* %t_pre, align 4
  %19 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %t_pre42 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %19, i32 0, i32 4
  store i32 %18, i32* %t_pre42, align 8
  br label %if.end.64

if.else.43:                                       ; preds = %if.else.32
  %arraydecay44 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %call45 = call i32 @strcmp(i8* %arraydecay44, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.10, i32 0, i32 0)) #8
  %tobool46 = icmp ne i32 %call45, 0
  br i1 %tobool46, label %if.else.53, label %if.then.47

if.then.47:                                       ; preds = %if.else.43
  %arraydecay49 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call50 = call i32 (i8*, i8*, ...) @__isoc99_sscanf(i8* %arraydecay49, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.6, i32 0, i32 0), i32* %op48, i32* %f_tp) #7
  %20 = load i32, i32* %op48, align 4
  %21 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %op51 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %21, i32 0, i32 1
  store i32 %20, i32* %op51, align 4
  %22 = load i32, i32* %f_tp, align 4
  %23 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %f_tp52 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %23, i32 0, i32 5
  store i32 %22, i32* %f_tp52, align 4
  br label %if.end.63

if.else.53:                                       ; preds = %if.else.43
  %arraydecay54 = getelementptr inbounds [4 x i8], [4 x i8]* %type, i32 0, i32 0
  %call55 = call i32 @strcmp(i8* %arraydecay54, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i32 0, i32 0)) #8
  %tobool56 = icmp ne i32 %call55, 0
  br i1 %tobool56, label %if.end.62, label %if.then.57

if.then.57:                                       ; preds = %if.else.53
  %arraydecay59 = getelementptr inbounds [20 x i8], [20 x i8]* %tail, i32 0, i32 0
  %call60 = call i32 (i8*, i8*, ...) @__isoc99_sscanf(i8* %arraydecay59, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.12, i32 0, i32 0), i32* %op58, i32* %op_i, i64* %s_c, i64* %t_c) #7
  %24 = load i32, i32* %op58, align 4
  %25 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %op61 = getelementptr inbounds %struct.Mutation, %struct.Mutation* %25, i32 0, i32 1
  store i32 %24, i32* %op61, align 4
  %26 = load i32, i32* %op_i, align 4
  %27 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %op_index = getelementptr inbounds %struct.Mutation, %struct.Mutation* %27, i32 0, i32 6
  store i32 %26, i32* %op_index, align 8
  %28 = load i64, i64* %s_c, align 8
  %29 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %s_con = getelementptr inbounds %struct.Mutation, %struct.Mutation* %29, i32 0, i32 7
  store i64 %28, i64* %s_con, align 8
  %30 = load i64, i64* %t_c, align 8
  %31 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %t_con = getelementptr inbounds %struct.Mutation, %struct.Mutation* %31, i32 0, i32 8
  store i64 %30, i64* %t_con, align 8
  br label %if.end.62

if.end.62:                                        ; preds = %if.then.57, %if.else.53
  br label %if.end.63

if.end.63:                                        ; preds = %if.end.62, %if.then.47
  br label %if.end.64

if.end.64:                                        ; preds = %if.end.63, %if.then.36
  br label %if.end.65

if.end.65:                                        ; preds = %if.end.64, %if.then.25
  br label %if.end.66

if.end.66:                                        ; preds = %if.end.65, %if.then.17
  %32 = load %struct.Mutation*, %struct.Mutation** %m, align 8
  %33 = load i32, i32* %id, align 4
  %idxprom = sext i32 %33 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  store %struct.Mutation* %32, %struct.Mutation** %arrayidx, align 8
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %call67 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str.13, i32 0, i32 0))
  ret void
}

; Function Attrs: nounwind argmemonly
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

declare %struct._IO_FILE* @fopen(i8*, i8*) #2

declare i32 @printf(i8*, ...) #2

; Function Attrs: noreturn nounwind
declare void @exit(i32) #3

declare i8* @fgets(i8*, i32, %struct._IO_FILE*) #2

; Function Attrs: nounwind
declare i32 @__isoc99_sscanf(i8*, i8*, ...) #4

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #4

; Function Attrs: nounwind readonly
declare i32 @strcmp(i8*, i8*) #5

; Function Attrs: nounwind uwtable
define i32 @__accmut__state_changed() #0 {
entry:
  ret i32 0
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__process_call_i32() #0 {
entry:
  ret i32 0
}

; Function Attrs: nounwind uwtable
define i64 @__accmut__process_call_i64() #0 {
entry:
  ret i64 0
}

; Function Attrs: nounwind uwtable
define void @__accmut__process_call_void() #0 {
entry:
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__cal_i32_arith(i32 %op, i32 %a, i32 %b) #0 {
entry:
  %retval = alloca i32, align 4
  %op.addr = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %op, i32* %op.addr, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %0 = load i32, i32* %op.addr, align 4
  switch i32 %0, label %sw.default [
    i32 14, label %sw.bb
    i32 16, label %sw.bb.1
    i32 18, label %sw.bb.2
    i32 20, label %sw.bb.3
    i32 21, label %sw.bb.4
    i32 23, label %sw.bb.6
    i32 24, label %sw.bb.7
    i32 26, label %sw.bb.9
    i32 27, label %sw.bb.10
    i32 28, label %sw.bb.11
    i32 29, label %sw.bb.13
    i32 30, label %sw.bb.14
    i32 31, label %sw.bb.15
  ]

sw.bb:                                            ; preds = %entry
  %1 = load i32, i32* %a.addr, align 4
  %2 = load i32, i32* %b.addr, align 4
  %add = add nsw i32 %1, %2
  store i32 %add, i32* %retval, align 4
  br label %return

sw.bb.1:                                          ; preds = %entry
  %3 = load i32, i32* %a.addr, align 4
  %4 = load i32, i32* %b.addr, align 4
  %sub = sub nsw i32 %3, %4
  store i32 %sub, i32* %retval, align 4
  br label %return

sw.bb.2:                                          ; preds = %entry
  %5 = load i32, i32* %a.addr, align 4
  %6 = load i32, i32* %b.addr, align 4
  %mul = mul nsw i32 %5, %6
  store i32 %mul, i32* %retval, align 4
  br label %return

sw.bb.3:                                          ; preds = %entry
  %7 = load i32, i32* %a.addr, align 4
  %8 = load i32, i32* %b.addr, align 4
  %div = udiv i32 %7, %8
  store i32 %div, i32* %retval, align 4
  br label %return

sw.bb.4:                                          ; preds = %entry
  %9 = load i32, i32* %a.addr, align 4
  %10 = load i32, i32* %b.addr, align 4
  %div5 = sdiv i32 %9, %10
  store i32 %div5, i32* %retval, align 4
  br label %return

sw.bb.6:                                          ; preds = %entry
  %11 = load i32, i32* %a.addr, align 4
  %12 = load i32, i32* %b.addr, align 4
  %rem = urem i32 %11, %12
  store i32 %rem, i32* %retval, align 4
  br label %return

sw.bb.7:                                          ; preds = %entry
  %13 = load i32, i32* %a.addr, align 4
  %14 = load i32, i32* %b.addr, align 4
  %rem8 = srem i32 %13, %14
  store i32 %rem8, i32* %retval, align 4
  br label %return

sw.bb.9:                                          ; preds = %entry
  %15 = load i32, i32* %a.addr, align 4
  %16 = load i32, i32* %b.addr, align 4
  %shl = shl i32 %15, %16
  store i32 %shl, i32* %retval, align 4
  br label %return

sw.bb.10:                                         ; preds = %entry
  %17 = load i32, i32* %a.addr, align 4
  %18 = load i32, i32* %b.addr, align 4
  %shr = lshr i32 %17, %18
  store i32 %shr, i32* %retval, align 4
  br label %return

sw.bb.11:                                         ; preds = %entry
  %19 = load i32, i32* %a.addr, align 4
  %20 = load i32, i32* %b.addr, align 4
  %shr12 = ashr i32 %19, %20
  store i32 %shr12, i32* %retval, align 4
  br label %return

sw.bb.13:                                         ; preds = %entry
  %21 = load i32, i32* %a.addr, align 4
  %22 = load i32, i32* %b.addr, align 4
  %and = and i32 %21, %22
  store i32 %and, i32* %retval, align 4
  br label %return

sw.bb.14:                                         ; preds = %entry
  %23 = load i32, i32* %a.addr, align 4
  %24 = load i32, i32* %b.addr, align 4
  %or = or i32 %23, %24
  store i32 %or, i32* %retval, align 4
  br label %return

sw.bb.15:                                         ; preds = %entry
  %25 = load i32, i32* %a.addr, align 4
  %26 = load i32, i32* %b.addr, align 4
  %xor = xor i32 %25, %26
  store i32 %xor, i32* %retval, align 4
  br label %return

sw.default:                                       ; preds = %entry
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.14, i32 0, i32 0))
  call void @exit(i32 0) #6
  unreachable

return:                                           ; preds = %sw.bb.15, %sw.bb.14, %sw.bb.13, %sw.bb.11, %sw.bb.10, %sw.bb.9, %sw.bb.7, %sw.bb.6, %sw.bb.4, %sw.bb.3, %sw.bb.2, %sw.bb.1, %sw.bb
  %27 = load i32, i32* %retval, align 4
  ret i32 %27
}

; Function Attrs: nounwind uwtable
define i64 @__accmut__cal_i64_arith(i32 %op, i64 %a, i64 %b) #0 {
entry:
  %retval = alloca i64, align 8
  %op.addr = alloca i32, align 4
  %a.addr = alloca i64, align 8
  %b.addr = alloca i64, align 8
  store i32 %op, i32* %op.addr, align 4
  store i64 %a, i64* %a.addr, align 8
  store i64 %b, i64* %b.addr, align 8
  %0 = load i32, i32* %op.addr, align 4
  switch i32 %0, label %sw.default [
    i32 14, label %sw.bb
    i32 16, label %sw.bb.1
    i32 18, label %sw.bb.2
    i32 20, label %sw.bb.3
    i32 21, label %sw.bb.4
    i32 23, label %sw.bb.6
    i32 24, label %sw.bb.7
    i32 26, label %sw.bb.9
    i32 27, label %sw.bb.10
    i32 28, label %sw.bb.11
    i32 29, label %sw.bb.13
    i32 30, label %sw.bb.14
    i32 31, label %sw.bb.15
  ]

sw.bb:                                            ; preds = %entry
  %1 = load i64, i64* %a.addr, align 8
  %2 = load i64, i64* %b.addr, align 8
  %add = add nsw i64 %1, %2
  store i64 %add, i64* %retval, align 8
  br label %return

sw.bb.1:                                          ; preds = %entry
  %3 = load i64, i64* %a.addr, align 8
  %4 = load i64, i64* %b.addr, align 8
  %sub = sub nsw i64 %3, %4
  store i64 %sub, i64* %retval, align 8
  br label %return

sw.bb.2:                                          ; preds = %entry
  %5 = load i64, i64* %a.addr, align 8
  %6 = load i64, i64* %b.addr, align 8
  %mul = mul nsw i64 %5, %6
  store i64 %mul, i64* %retval, align 8
  br label %return

sw.bb.3:                                          ; preds = %entry
  %7 = load i64, i64* %a.addr, align 8
  %8 = load i64, i64* %b.addr, align 8
  %div = udiv i64 %7, %8
  store i64 %div, i64* %retval, align 8
  br label %return

sw.bb.4:                                          ; preds = %entry
  %9 = load i64, i64* %a.addr, align 8
  %10 = load i64, i64* %b.addr, align 8
  %div5 = sdiv i64 %9, %10
  store i64 %div5, i64* %retval, align 8
  br label %return

sw.bb.6:                                          ; preds = %entry
  %11 = load i64, i64* %a.addr, align 8
  %12 = load i64, i64* %b.addr, align 8
  %rem = urem i64 %11, %12
  store i64 %rem, i64* %retval, align 8
  br label %return

sw.bb.7:                                          ; preds = %entry
  %13 = load i64, i64* %a.addr, align 8
  %14 = load i64, i64* %b.addr, align 8
  %rem8 = srem i64 %13, %14
  store i64 %rem8, i64* %retval, align 8
  br label %return

sw.bb.9:                                          ; preds = %entry
  %15 = load i64, i64* %a.addr, align 8
  %16 = load i64, i64* %b.addr, align 8
  %shl = shl i64 %15, %16
  store i64 %shl, i64* %retval, align 8
  br label %return

sw.bb.10:                                         ; preds = %entry
  %17 = load i64, i64* %a.addr, align 8
  %18 = load i64, i64* %b.addr, align 8
  %shr = lshr i64 %17, %18
  store i64 %shr, i64* %retval, align 8
  br label %return

sw.bb.11:                                         ; preds = %entry
  %19 = load i64, i64* %a.addr, align 8
  %20 = load i64, i64* %b.addr, align 8
  %shr12 = ashr i64 %19, %20
  store i64 %shr12, i64* %retval, align 8
  br label %return

sw.bb.13:                                         ; preds = %entry
  %21 = load i64, i64* %a.addr, align 8
  %22 = load i64, i64* %b.addr, align 8
  %and = and i64 %21, %22
  store i64 %and, i64* %retval, align 8
  br label %return

sw.bb.14:                                         ; preds = %entry
  %23 = load i64, i64* %a.addr, align 8
  %24 = load i64, i64* %b.addr, align 8
  %or = or i64 %23, %24
  store i64 %or, i64* %retval, align 8
  br label %return

sw.bb.15:                                         ; preds = %entry
  %25 = load i64, i64* %a.addr, align 8
  %26 = load i64, i64* %b.addr, align 8
  %xor = xor i64 %25, %26
  store i64 %xor, i64* %retval, align 8
  br label %return

sw.default:                                       ; preds = %entry
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.15, i32 0, i32 0))
  call void @exit(i32 0) #6
  unreachable

return:                                           ; preds = %sw.bb.15, %sw.bb.14, %sw.bb.13, %sw.bb.11, %sw.bb.10, %sw.bb.9, %sw.bb.7, %sw.bb.6, %sw.bb.4, %sw.bb.3, %sw.bb.2, %sw.bb.1, %sw.bb
  %27 = load i64, i64* %retval, align 8
  ret i64 %27
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__fork(i32 %mutid) #0 {
entry:
  %mutid.addr = alloca i32, align 4
  %pid = alloca i32, align 4
  store i32 %mutid, i32* %mutid.addr, align 4
  store i32 0, i32* %pid, align 4
  %0 = load i32, i32* @MUTATION_ID, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %if.then, label %if.end.8

if.then:                                          ; preds = %entry
  %call = call i32 @fork() #7
  store i32 %call, i32* %pid, align 4
  %1 = load i32, i32* %pid, align 4
  %cmp1 = icmp slt i32 %1, 0
  br i1 %cmp1, label %if.then.2, label %if.else

if.then.2:                                        ; preds = %if.then
  %call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.16, i32 0, i32 0))
  call void @exit(i32 0) #6
  unreachable

if.else:                                          ; preds = %if.then
  %2 = load i32, i32* %pid, align 4
  %cmp4 = icmp eq i32 %2, 0
  br i1 %cmp4, label %if.then.5, label %if.else.6

if.then.5:                                        ; preds = %if.else
  br label %if.end

if.else.6:                                        ; preds = %if.else
  %3 = load i32, i32* %mutid.addr, align 4
  store i32 %3, i32* @MUTATION_ID, align 4
  br label %if.end

if.end:                                           ; preds = %if.else.6, %if.then.5
  br label %if.end.7

if.end.7:                                         ; preds = %if.end
  br label %if.end.8

if.end.8:                                         ; preds = %if.end.7, %entry
  %4 = load i32, i32* %pid, align 4
  ret i32 %4
}

; Function Attrs: nounwind
declare i32 @fork() #4

; Function Attrs: nounwind uwtable
define i32 @__accmut__process_i32_arith(i32 %from, i32 %to, i32 %left, i32 %right) #0 {
entry:
  %from.addr = alloca i32, align 4
  %to.addr = alloca i32, align 4
  %left.addr = alloca i32, align 4
  %right.addr = alloca i32, align 4
  %ori = alloca i32, align 4
  store i32 %from, i32* %from.addr, align 4
  store i32 %to, i32* %to.addr, align 4
  store i32 %left, i32* %left.addr, align 4
  store i32 %right, i32* %right.addr, align 4
  %0 = load i32, i32* %from.addr, align 4
  %idxprom = sext i32 %0 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  %1 = load %struct.Mutation*, %struct.Mutation** %arrayidx, align 8
  %op = getelementptr inbounds %struct.Mutation, %struct.Mutation* %1, i32 0, i32 1
  %2 = load i32, i32* %op, align 4
  %3 = load i32, i32* %left.addr, align 4
  %4 = load i32, i32* %right.addr, align 4
  %call = call i32 @__accmut__cal_i32_arith(i32 %2, i32 %3, i32 %4)
  store i32 %call, i32* %ori, align 4
  %5 = load i32, i32* %ori, align 4
  ret i32 %5
}

; Function Attrs: nounwind uwtable
define i64 @__accmut__process_i64_arith(i32 %from, i32 %to, i64 %left, i64 %right) #0 {
entry:
  %from.addr = alloca i32, align 4
  %to.addr = alloca i32, align 4
  %left.addr = alloca i64, align 8
  %right.addr = alloca i64, align 8
  %ori = alloca i64, align 8
  store i32 %from, i32* %from.addr, align 4
  store i32 %to, i32* %to.addr, align 4
  store i64 %left, i64* %left.addr, align 8
  store i64 %right, i64* %right.addr, align 8
  %0 = load i32, i32* %from.addr, align 4
  %idxprom = sext i32 %0 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  %1 = load %struct.Mutation*, %struct.Mutation** %arrayidx, align 8
  %op = getelementptr inbounds %struct.Mutation, %struct.Mutation* %1, i32 0, i32 1
  %2 = load i32, i32* %op, align 4
  %3 = load i64, i64* %left.addr, align 8
  %4 = load i64, i64* %right.addr, align 8
  %call = call i64 @__accmut__cal_i64_arith(i32 %2, i64 %3, i64 %4)
  store i64 %call, i64* %ori, align 8
  %5 = load i64, i64* %ori, align 8
  ret i64 %5
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__cal_i32_bool(i32 %pre, i32 %a, i32 %b) #0 {
entry:
  %retval = alloca i32, align 4
  %pre.addr = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %pre, i32* %pre.addr, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %0 = load i32, i32* %pre.addr, align 4
  switch i32 %0, label %sw.default [
    i32 32, label %sw.bb
    i32 33, label %sw.bb.1
    i32 34, label %sw.bb.4
    i32 35, label %sw.bb.7
    i32 36, label %sw.bb.10
    i32 37, label %sw.bb.13
    i32 38, label %sw.bb.16
    i32 39, label %sw.bb.19
    i32 40, label %sw.bb.22
    i32 41, label %sw.bb.25
  ]

sw.bb:                                            ; preds = %entry
  %1 = load i32, i32* %a.addr, align 4
  %2 = load i32, i32* %b.addr, align 4
  %cmp = icmp eq i32 %1, %2
  %conv = zext i1 %cmp to i32
  store i32 %conv, i32* %retval, align 4
  br label %return

sw.bb.1:                                          ; preds = %entry
  %3 = load i32, i32* %a.addr, align 4
  %4 = load i32, i32* %b.addr, align 4
  %cmp2 = icmp ne i32 %3, %4
  %conv3 = zext i1 %cmp2 to i32
  store i32 %conv3, i32* %retval, align 4
  br label %return

sw.bb.4:                                          ; preds = %entry
  %5 = load i32, i32* %a.addr, align 4
  %6 = load i32, i32* %b.addr, align 4
  %cmp5 = icmp ugt i32 %5, %6
  %conv6 = zext i1 %cmp5 to i32
  store i32 %conv6, i32* %retval, align 4
  br label %return

sw.bb.7:                                          ; preds = %entry
  %7 = load i32, i32* %a.addr, align 4
  %8 = load i32, i32* %b.addr, align 4
  %cmp8 = icmp uge i32 %7, %8
  %conv9 = zext i1 %cmp8 to i32
  store i32 %conv9, i32* %retval, align 4
  br label %return

sw.bb.10:                                         ; preds = %entry
  %9 = load i32, i32* %a.addr, align 4
  %10 = load i32, i32* %b.addr, align 4
  %cmp11 = icmp ult i32 %9, %10
  %conv12 = zext i1 %cmp11 to i32
  store i32 %conv12, i32* %retval, align 4
  br label %return

sw.bb.13:                                         ; preds = %entry
  %11 = load i32, i32* %a.addr, align 4
  %12 = load i32, i32* %b.addr, align 4
  %cmp14 = icmp ule i32 %11, %12
  %conv15 = zext i1 %cmp14 to i32
  store i32 %conv15, i32* %retval, align 4
  br label %return

sw.bb.16:                                         ; preds = %entry
  %13 = load i32, i32* %a.addr, align 4
  %14 = load i32, i32* %b.addr, align 4
  %cmp17 = icmp sgt i32 %13, %14
  %conv18 = zext i1 %cmp17 to i32
  store i32 %conv18, i32* %retval, align 4
  br label %return

sw.bb.19:                                         ; preds = %entry
  %15 = load i32, i32* %a.addr, align 4
  %16 = load i32, i32* %b.addr, align 4
  %cmp20 = icmp sge i32 %15, %16
  %conv21 = zext i1 %cmp20 to i32
  store i32 %conv21, i32* %retval, align 4
  br label %return

sw.bb.22:                                         ; preds = %entry
  %17 = load i32, i32* %a.addr, align 4
  %18 = load i32, i32* %b.addr, align 4
  %cmp23 = icmp slt i32 %17, %18
  %conv24 = zext i1 %cmp23 to i32
  store i32 %conv24, i32* %retval, align 4
  br label %return

sw.bb.25:                                         ; preds = %entry
  %19 = load i32, i32* %a.addr, align 4
  %20 = load i32, i32* %b.addr, align 4
  %cmp26 = icmp sle i32 %19, %20
  %conv27 = zext i1 %cmp26 to i32
  store i32 %conv27, i32* %retval, align 4
  br label %return

sw.default:                                       ; preds = %entry
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([35 x i8], [35 x i8]* @.str.17, i32 0, i32 0))
  call void @exit(i32 0) #6
  unreachable

return:                                           ; preds = %sw.bb.25, %sw.bb.22, %sw.bb.19, %sw.bb.16, %sw.bb.13, %sw.bb.10, %sw.bb.7, %sw.bb.4, %sw.bb.1, %sw.bb
  %21 = load i32, i32* %retval, align 4
  ret i32 %21
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__cal_i64_bool(i32 %pre, i64 %a, i64 %b) #0 {
entry:
  %retval = alloca i32, align 4
  %pre.addr = alloca i32, align 4
  %a.addr = alloca i64, align 8
  %b.addr = alloca i64, align 8
  store i32 %pre, i32* %pre.addr, align 4
  store i64 %a, i64* %a.addr, align 8
  store i64 %b, i64* %b.addr, align 8
  %0 = load i32, i32* %pre.addr, align 4
  switch i32 %0, label %sw.default [
    i32 32, label %sw.bb
    i32 33, label %sw.bb.1
    i32 34, label %sw.bb.4
    i32 35, label %sw.bb.7
    i32 36, label %sw.bb.10
    i32 37, label %sw.bb.13
    i32 38, label %sw.bb.16
    i32 39, label %sw.bb.19
    i32 40, label %sw.bb.22
    i32 41, label %sw.bb.25
  ]

sw.bb:                                            ; preds = %entry
  %1 = load i64, i64* %a.addr, align 8
  %2 = load i64, i64* %b.addr, align 8
  %cmp = icmp eq i64 %1, %2
  %conv = zext i1 %cmp to i32
  store i32 %conv, i32* %retval, align 4
  br label %return

sw.bb.1:                                          ; preds = %entry
  %3 = load i64, i64* %a.addr, align 8
  %4 = load i64, i64* %b.addr, align 8
  %cmp2 = icmp ne i64 %3, %4
  %conv3 = zext i1 %cmp2 to i32
  store i32 %conv3, i32* %retval, align 4
  br label %return

sw.bb.4:                                          ; preds = %entry
  %5 = load i64, i64* %a.addr, align 8
  %6 = load i64, i64* %b.addr, align 8
  %cmp5 = icmp ugt i64 %5, %6
  %conv6 = zext i1 %cmp5 to i32
  store i32 %conv6, i32* %retval, align 4
  br label %return

sw.bb.7:                                          ; preds = %entry
  %7 = load i64, i64* %a.addr, align 8
  %8 = load i64, i64* %b.addr, align 8
  %cmp8 = icmp uge i64 %7, %8
  %conv9 = zext i1 %cmp8 to i32
  store i32 %conv9, i32* %retval, align 4
  br label %return

sw.bb.10:                                         ; preds = %entry
  %9 = load i64, i64* %a.addr, align 8
  %10 = load i64, i64* %b.addr, align 8
  %cmp11 = icmp ult i64 %9, %10
  %conv12 = zext i1 %cmp11 to i32
  store i32 %conv12, i32* %retval, align 4
  br label %return

sw.bb.13:                                         ; preds = %entry
  %11 = load i64, i64* %a.addr, align 8
  %12 = load i64, i64* %b.addr, align 8
  %cmp14 = icmp ule i64 %11, %12
  %conv15 = zext i1 %cmp14 to i32
  store i32 %conv15, i32* %retval, align 4
  br label %return

sw.bb.16:                                         ; preds = %entry
  %13 = load i64, i64* %a.addr, align 8
  %14 = load i64, i64* %b.addr, align 8
  %cmp17 = icmp sgt i64 %13, %14
  %conv18 = zext i1 %cmp17 to i32
  store i32 %conv18, i32* %retval, align 4
  br label %return

sw.bb.19:                                         ; preds = %entry
  %15 = load i64, i64* %a.addr, align 8
  %16 = load i64, i64* %b.addr, align 8
  %cmp20 = icmp sge i64 %15, %16
  %conv21 = zext i1 %cmp20 to i32
  store i32 %conv21, i32* %retval, align 4
  br label %return

sw.bb.22:                                         ; preds = %entry
  %17 = load i64, i64* %a.addr, align 8
  %18 = load i64, i64* %b.addr, align 8
  %cmp23 = icmp slt i64 %17, %18
  %conv24 = zext i1 %cmp23 to i32
  store i32 %conv24, i32* %retval, align 4
  br label %return

sw.bb.25:                                         ; preds = %entry
  %19 = load i64, i64* %a.addr, align 8
  %20 = load i64, i64* %b.addr, align 8
  %cmp26 = icmp sle i64 %19, %20
  %conv27 = zext i1 %cmp26 to i32
  store i32 %conv27, i32* %retval, align 4
  br label %return

sw.default:                                       ; preds = %entry
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @.str.18, i32 0, i32 0))
  call void @exit(i32 0) #6
  unreachable

return:                                           ; preds = %sw.bb.25, %sw.bb.22, %sw.bb.19, %sw.bb.16, %sw.bb.13, %sw.bb.10, %sw.bb.7, %sw.bb.4, %sw.bb.1, %sw.bb
  %21 = load i32, i32* %retval, align 4
  ret i32 %21
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__process_i32_cmp(i32 %from, i32 %to, i32 %left, i32 %right) #0 {
entry:
  %from.addr = alloca i32, align 4
  %to.addr = alloca i32, align 4
  %left.addr = alloca i32, align 4
  %right.addr = alloca i32, align 4
  %ori = alloca i32, align 4
  store i32 %from, i32* %from.addr, align 4
  store i32 %to, i32* %to.addr, align 4
  store i32 %left, i32* %left.addr, align 4
  store i32 %right, i32* %right.addr, align 4
  %0 = load i32, i32* %from.addr, align 4
  %idxprom = sext i32 %0 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  %1 = load %struct.Mutation*, %struct.Mutation** %arrayidx, align 8
  %s_pre = getelementptr inbounds %struct.Mutation, %struct.Mutation* %1, i32 0, i32 3
  %2 = load i32, i32* %s_pre, align 4
  %3 = load i32, i32* %left.addr, align 4
  %4 = load i32, i32* %right.addr, align 4
  %call = call i32 @__accmut__cal_i32_bool(i32 %2, i32 %3, i32 %4)
  store i32 %call, i32* %ori, align 4
  %5 = load i32, i32* %ori, align 4
  ret i32 %5
}

; Function Attrs: nounwind uwtable
define i32 @__accmut__process_i64_cmp(i32 %from, i32 %to, i64 %left, i64 %right) #0 {
entry:
  %from.addr = alloca i32, align 4
  %to.addr = alloca i32, align 4
  %left.addr = alloca i64, align 8
  %right.addr = alloca i64, align 8
  %ori = alloca i64, align 8
  store i32 %from, i32* %from.addr, align 4
  store i32 %to, i32* %to.addr, align 4
  store i64 %left, i64* %left.addr, align 8
  store i64 %right, i64* %right.addr, align 8
  %0 = load i32, i32* %from.addr, align 4
  %idxprom = sext i32 %0 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  %1 = load %struct.Mutation*, %struct.Mutation** %arrayidx, align 8
  %s_pre = getelementptr inbounds %struct.Mutation, %struct.Mutation* %1, i32 0, i32 3
  %2 = load i32, i32* %s_pre, align 4
  %3 = load i64, i64* %left.addr, align 8
  %4 = load i64, i64* %right.addr, align 8
  %call = call i32 @__accmut__cal_i64_bool(i32 %2, i64 %3, i64 %4)
  %conv = sext i32 %call to i64
  store i64 %conv, i64* %ori, align 8
  %5 = load i64, i64* %ori, align 8
  %conv1 = trunc i64 %5 to i32
  ret i32 %conv1
}

; Function Attrs: nounwind uwtable
define void @__accmut__process_st_i32(i32 %from, i32 %to, i32* %addr) #0 {
entry:
  %from.addr = alloca i32, align 4
  %to.addr = alloca i32, align 4
  %addr.addr = alloca i32*, align 8
  store i32 %from, i32* %from.addr, align 4
  store i32 %to, i32* %to.addr, align 4
  store i32* %addr, i32** %addr.addr, align 8
  %0 = load i32, i32* %from.addr, align 4
  %idxprom = sext i32 %0 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  %1 = load %struct.Mutation*, %struct.Mutation** %arrayidx, align 8
  %s_con = getelementptr inbounds %struct.Mutation, %struct.Mutation* %1, i32 0, i32 7
  %2 = load i64, i64* %s_con, align 8
  %conv = trunc i64 %2 to i32
  %3 = load i32*, i32** %addr.addr, align 8
  store i32 %conv, i32* %3, align 4
  ret void
}

; Function Attrs: nounwind uwtable
define void @__accmut__process_st_i64(i32 %from, i32 %to, i64* %addr) #0 {
entry:
  %from.addr = alloca i32, align 4
  %to.addr = alloca i32, align 4
  %addr.addr = alloca i64*, align 8
  store i32 %from, i32* %from.addr, align 4
  store i32 %to, i32* %to.addr, align 4
  store i64* %addr, i64** %addr.addr, align 8
  %0 = load i32, i32* %from.addr, align 4
  %idxprom = sext i32 %0 to i64
  %arrayidx = getelementptr inbounds [10001 x %struct.Mutation*], [10001 x %struct.Mutation*]* @ALLMUTS, i64 0, i64 %idxprom
  %1 = load %struct.Mutation*, %struct.Mutation** %arrayidx, align 8
  %s_con = getelementptr inbounds %struct.Mutation, %struct.Mutation* %1, i32 0, i32 7
  %2 = load i64, i64* %s_con, align 8
  %3 = load i64*, i64** %addr.addr, align 8
  store i64 %2, i64* %3, align 8
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @test(i32 %a, i32 %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  call void @__accmut__process_st_i32(i32 1, i32 5, i32* %c)
  %0 = load i32, i32* %a.addr, align 4
  %1 = load i32, i32* %b.addr, align 4
  %add = call i32 @__accmut__process_i32_arith(i32 6, i32 11, i32 %0, i32 %1)
  %2 = load i32, i32* %c, align 4
  %add1 = call i32 @__accmut__process_i32_arith(i32 12, i32 17, i32 %add, i32 %2)
  ret i32 %add1
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
entry:
  call void @__accmut__init()
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %res = alloca i32, align 4
  %c = alloca i64, align 8
  %d = alloca i64, align 8
  store i32 0, i32* %retval, align 4
  store i32 1, i32* %a, align 4
  store i32 2, i32* %b, align 4
  %0 = load i32, i32* %a, align 4
  %1 = load i32, i32* %b, align 4
  %call = call i32 @test(i32 %0, i32 %1)
  store i32 %call, i32* %res, align 4
  %2 = load i32, i32* %res, align 4
  %call1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.19, i32 0, i32 0), i32 %2)
  store i64 888, i64* %c, align 8
  store i64 999, i64* %d, align 8
  ret i32 0
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind argmemonly }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind readonly "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noreturn nounwind }
attributes #7 = { nounwind }
attributes #8 = { nounwind readonly }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0  (https://github.com/wangbo15/accmut.git eeede8a285df0b3cbff367c9276426c7fc1e05bd)"}
