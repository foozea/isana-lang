=====================
Implimentation status
=====================

R5RS features
=============

Expressions
-----------
Primitive expression types
    Literal expressions
        * ``quote`` and ``'`` : **ok**

    Procedures
        * ``lambda`` : **ok**

    Conditionals
        * ``if`` : **ok**

    Assignments
        * ``set!`` : **ok**

Derived expression types
    Conditionals
        * ``cond`` : not yet
        * ``case`` : not yet
        * ``and`` : not yet
        * ``or`` : not yet

    Binding constructs
        * ``let`` : **ok**
        * ``let*`` : not yet
        * ``letrec`` : not yet

    Sequencing
        * ``begin`` : not yet

    Iteration
        * ``do`` : not yet
        * ``let`` (named let) : not yet

    Delayed evaluation
        * ``delay`` : not yet

    Quasiquotation
        * ``quasiquote`` and ````` : not yet

Macros
    Binding constructs for syntactic keywords
        * ``let-syntax`` : not yet
        * ``letrec-syntax`` : not yet

    Pattern language
        * ``syntax-rules`` : not yet

Program structure
-----------------
Definitions
    * ``define`` : **ok**

Syntax definitions
    * ``define-syntax`` : not yet

Standard procedures
-------------------
Equivalence predicates
    * ``eqv?`` : **ok**
    * ``eq?`` : **ok**
    * ``equal?`` : not yet

Numbers
    * ``number?`` : **ok**
    * ``complex?`` : **ok**
    * ``real?`` : **ok**
    * ``rational?`` : **ok**
    * ``integer?`` : **ok**
    * ``exact?`` : **ok**
    * ``inexact?`` : **ok**
    * ``=`` : not yet
    * ``>`` : not yet
    * ``>`` : not yet
    * ``<=`` : not yet
    * ``>=`` : not yet
    * ``zero?`` : not yet
    * ``positive?`` : not yet
    * ``negative?`` : not yet
    * ``odd?`` : not yet
    * ``even?`` : not yet
    * ``max`` : not yet
    * ``min`` : not yet
    * ``+`` : not yet
    * ``-`` : not yet
    * ``*`` : not yet
    * ``/`` : not yet
    * ``abs`` : not yet
    * ``quotient`` : not yet
    * ``remainder`` : not yet
    * ``modulo`` : not yet
    * ``gcd`` : not yet
    * ``lcm`` : not yet
    * ``numerator`` : not yet
    * ``denominator`` : not yet
    * ``floor`` : not yet
    * ``ceiling`` : not yet
    * ``truncate`` : not yet
    * ``round`` : not yet
    * ``exp`` : not yet
    * ``log`` : not yet
    * ``sin`` : not yet
    * ``cos`` : not yet
    * ``tan`` : not yet
    * ``asin`` : not yet
    * ``acos`` : not yet
    * ``atan`` : not yet
    * ``sqrt`` : not yet
    * ``expt`` : not yet
    * ``make-rectangular`` : not yet
    * ``make-polar`` : not yet
    * ``real-part`` : not yet
    * ``imag-part`` : not yet
    * ``magnitude`` : not yet
    * ``angle`` : not yet
    * ``exact->inexact`` : not yet
    * ``inexact->exact`` : not yet
    * ``number->string`` : not yet
    * ``string->number`` : not yet

Other data types
    Booleans
        * ``not`` : not yet
        * ``boolean?`` : **ok**

    Pairs and lists
        * ``pair?`` : **ok**
        * ``cons`` : **ok**
        * ``car`` : **ok**
        * ``cdr`` : **ok**
        * ``set-car!`` : **ok**
        * ``set-cdr!`` : **ok**
        * ``caar``,``cadr``, ... ``cddddr`` : not yet
        * ``null?`` : **ok**
        * ``list?`` : not yet
        * ``list`` : **ok**
        * ``length`` : not yet
        * ``append`` : not yet
        * ``reverse`` : not yet
        * ``list-tail`` : not yet
        * ``list-ref`` : not yet
        * ``memq`` : not yet
        * ``memv`` : not yet
        * ``member`` : not yet
        * ``assq`` : not yet
        * ``assv`` : not yet
        * ``assoc`` : not yet

    Symbols
        * ``symbol?`` : **ok**
        * ``symbol->string`` : not yet
        * ``string->symbol`` : not yet

    Characters
        * ``char?`` : not yet
        * ``char=?`` : not yet
        * ``char<?`` : not yet
        * ``char>?`` : not yet
        * ``char<=?`` : not yet
        * ``char>=?`` : not yet
        * ``char-ci=?`` : not yet
        * ``char-ci<?`` : not yet
        * ``char-ci>?`` : not yet
        * ``char-ci<=?`` : not yet
        * ``char-ci>=?`` : not yet
        * ``char-alphabetic?`` : not yet
        * ``char-numeric?`` : not yet
        * ``char-whitespace?`` : not yet
        * ``char-upper-case?`` : not yet
        * ``char-lower-case?`` : not yet
        * ``char->integer`` : not yet
        * ``integer->char`` : not yet
        * ``char-upcase`` : not yet
        * ``char-downcase`` : not yet

    Strings
        * ``string?`` : **ok**
        * ``make-string`` : not yet
        * ``string`` : not yet
        * ``string-length`` : not yet
        * ``string-ref`` : not yet
        * ``string-set!`` : not yet
        * ``string=?`` : not yet
        * ``string-ci=?`` : not yet
        * ``string<?`` : not yet
        * ``string>?`` : not yet
        * ``string<=?`` : not yet
        * ``string>=?`` : not yet
        * ``string-ci<?`` : not yet
        * ``string-ci>?`` : not yet
        * ``string-ci<=?`` : not yet
        * ``string-ci>=?`` : not yet
        * ``substring`` : not yet
        * ``string-append`` : not yet
        * ``string-list`` : not yet
        * ``list->string`` : not yet
        * ``string-copy`` : not yet
        * ``string-fill!`` : not yet

    Vectors
        * ``vector?`` : not yet
        * ``make-vector`` : not yet
        * ``vector`` : not yet
        * ``vector-length`` : not yet
        * ``vector-ref`` : not yet
        * ``vector-set!`` : not yet
        * ``vector->list`` : not yet
        * ``list->vector`` : not yet
        * ``vector-fill!`` : not yet

Control features
    * ``procedure?`` : **ok**
    * ``apply`` : not yet
    * ``map`` : not yet
    * ``for-each`` : not yet
    * ``force`` : not yet
    * ``call-with-current-continuation`` : not yet
    * ``values`` : not yet
    * ``call-with-values`` : not yet
    * ``dynamic-wind`` : not yet

Eval
    * ``eval`` : not yet
    * ``scheme-report-environment`` : not yet
    * ``null-environment`` : not yet
    * ``interaction-environment`` : not yet

Input and output
    Ports
        * ``call-with-input-file`` : not yet
        * ``call-with-output-file`` : not yet
        * ``input-port?`` : not yet
        * ``output-port?`` : not yet
        * ``current-input-port`` : not yet
        * ``current-output-port`` : not yet
        * ``with-input-from-file`` : not yet
        * ``with-output-to-file`` : not yet
        * ``open-input-file`` : not yet
        * ``open-output-file`` : not yet
        * ``close-input-port`` : not yet
        * ``close-output-port`` : not yet

    Input
        * ``read`` : not yet
        * ``read-char`` : not yet
        * ``peek-char`` : not yet
        * ``eof-object?`` : not yet
        * ``char-ready?`` : not yet

    Output
        * ``write`` : not yet
        * ``display`` : **ok**
        * ``newline`` : **ok**
        * ``write-char`` : not yet

    System interface
        * ``load`` : not yet
        * ``transcript-on`` : not yet
        * ``transcript-off`` : not yet

