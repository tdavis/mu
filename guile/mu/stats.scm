;;
;; Copyright (C) 2011-2012 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
;;
;; This program is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published by the
;; Free Software Foundation; either version 3, or (at your option) any
;; later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;

;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software Foundation,
;; Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

(define-module (mu stats)
  :use-module (oop goops)
  :use-module (mu)
  :use-module (srfi srfi-1)
  :use-module (ice-9 i18n)
  :use-module (ice-9 r5rs)
  :export ( mu:tabulate
	    mu:average
	    mu:stddev
	    mu:max
	    mu:min
	    mu:weekday-numbers->names
	    mu:month-numbers->names))


(define* (mu:tabulate func #:optional (expr #t))
  "Execute FUNC for each message matching EXPR, and return an alist
with maps each result of FUNC to its frequency. FUNC is a function
takes a <mu-message> instance as its argument. For example, to
tabulate messages by weekday, one could use:
   (mu:tabulate (lambda(msg) (tm:wday (localtime (date msg))))), and
get back a list like
   ((1 . 2) (2 . 5)(3 . 4)(4 . 4)(5 . 12)(6 . 7)(7. 2))."
  (let ((table '()))
    (mu:for-each-message
      (lambda(msg)
	(let* ((val (func msg))
		(old-freq (or (assoc-ref table val) 0)))
	  (set! table (assoc-set! table val (1+ old-freq)))))
      expr)
    table))
 
(define (average lst)
  "Calculate the average of a list LST of numbers, or #f if undefined."
  (if (null? lst)
    #f
    (/ (apply + lst) (length lst))))

(define (stddev lst)
  "Calculate the standard deviation of a list LST of numbers or #f if
undefined."
  (let* ((avg (average lst))
	  (sosq (if avg
		  (apply + (map (lambda (x)(* (- x avg) (- x avg))) lst)))))
    (if sosq
      (sqrt (/ sosq (length lst))))))


(define* (mu:average func #:optional (expr #t))
  "Get the average value of FUNC applied to all messages matching
EXPR (or #t for all). Returns #f if undefined."
  (average (map func (mu:message-list expr))))

(define* (mu:stddev func #:optional (expr #t))
  "Get the standard deviation the the values of FUNC applied to all
messages matching EXPR (or #t for all). This is the 'population' stddev, not the 'sample' stddev. Returns #f if undefined."
  (stddev (map func (mu:message-list expr))))
 
(define* (mu:max func #:optional (expr #t))
  "Get the maximum value of FUNC applied to all messages matching
EXPR (or #t for all). Returns #f if undefined."
  (apply max (map func (mu:message-list expr))))

(define* (mu:min func #:optional (expr #t))
  "Get the minimum value of FUNC applied to all messages matching
EXPR (or #t for all). Returns #f if undefined."
  (apply min (map func (mu:message-list expr))))
 
;; a list of abbreviated, localized day names
(define day-names
  (map locale-day-short (iota 7 1)))

(define (mu:weekday-numbers->names table)
  "Convert a list of pairs with the car denoting a day number (0-6)
into a list of pairs with the car replaced by the corresponding day
name (abbreviated) for the current locale."
    (map
      (lambda (pair)
	(cons (list-ref day-names (car pair)) (cdr pair)))
      table))

;; a list of abbreviated, localized month names
(define month-names
  (map locale-month-short (iota 12 1)))

(define (mu:month-numbers->names table)
    "Convert a list of pairs with the car denoting a month number (0-11)
into a list of pairs with the car replaced by the corresponding day
name (abbreviated)."
    (map
      (lambda (pair)
	(cons (list-ref month-names (car pair)) (cdr pair)))
      table))
