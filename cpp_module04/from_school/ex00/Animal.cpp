/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:47:51 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 17:23:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Animal.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

Animal::Animal() : type("Generic Animal") {
    LOG_CTOR();
}

Animal::Animal(const Animal& src) : type(src.type) {
    LOG_COPY();
}

Animal& Animal::operator=(const Animal& rhs) {
    LOG_ASSIGN();
    if (this != &rhs) {
        this->type = rhs.type;
    }
    return *this;
}

Animal::~Animal() {
    LOG_DTOR();
}

std::string Animal::getType() const { return this->type; }

void Animal::makeSound() const {
    LOG_METHOD("makeSound() called - unidentifiable noise");
}
